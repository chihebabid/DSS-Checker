#include <iostream>
#include "Petri/ConstructPetriFromFile.h"
#include "time.h"
#include <spot/twaalgos/dot.hh>
#include <CLI11.hpp>
#include <spot/twaalgos/translate.hh>
#include <spot/twaalgos/emptiness.hh>
#include "DSS/DSSBuilder.h"
#include "SpotMC/DSSKripke.h"
#include "SpotMC/ParseFormulaFile.h"
#include "SpotMC/Propositions.h"
using namespace std;


/**
* Save a graph in a dot file
*
* @param graph Graph to be saved
* @param filename Output filename
* @param options Options to customized the print
*/
void saveGraph(spot::twa_graph_ptr &graph, const string &filename, const char *options = nullptr) {
    fstream file;
    file.open(filename.c_str(), fstream::out);
    spot::print_dot(file, graph, options);
    file.close();
}


/**
 * Convert a formula into an automaton
 *
 * @param f  Formula to be converted
 * @param bdd  BDD structure
 * @param save_dot Save the automaton of the negated formula in a dot file
 * @return spot::twa_graph_ptr
 */
spot::twa_graph_ptr formula2Automaton(const spot::formula &f, spot::bdd_dict_ptr bdd, bool save_dot = false) {
    spot::translator tmp = spot::translator(bdd);
    tmp.set_type(spot::postprocessor::BA);
    spot::twa_graph_ptr af = tmp.run(f);

    // save the generated automaton in a dot file
    if (save_dot) {
        saveGraph(af, "negated_formula.dot");
    }

    return af;
}

namespace {
const string RULE(72, '=');
const string THIN_RULE(72, '-');
}


int main(int argc, char *argv[]) {
    CLI::App app{"DSS-Checker : Distributed State Space Checker"};

    string file_name;
    app.add_option("--file", file_name, "Petri net model file")
            ->type_name("Path")
            ->required()
            ->check(CLI::ExistingFile);

    string algorithm = "DSS";
    app.add_option("--algorithm", algorithm, "DSS building algorithm (default: DSS)")->group("Algorithm");

    bool fairness{false};
    app.add_flag("--fairness,!--no-fairness", fairness,
                 "Model checking under fairness: a module waiting for a synchronisation only stops forever in a "
                 "sync-closed SCC (default: disabled)")->group("Algorithm");

    bool reduction{true};
    app.add_flag("--reduction,!--no-reduction", reduction,
                 "Reduce the metastates by bisimulation (default: enabled)")->group("Algorithm");

    bool dot_output{false};
    app.add_flag("--dot-output", dot_output, "Save the output graph in a dot file")->group("Print");

    bool svg_output{false};
    app.add_flag("--svg-output", svg_output, "Render the output graph in an SVG file (requires Graphviz)")->group("Print");

    bool txt_output{false};
    app.add_flag("--txt-output", txt_output, "Save the DSS statistics in a text file (model.pn.txt)")->group("Print");


    string property_file;
    app.add_option("--mc", property_file, "LTL property file")
            ->type_name("Path")
            ->check(CLI::ExistingFile);

    bool verbose{false};
    app.add_flag("--verbose", verbose, "List every place, transition and arc of the model while loading it")->group("Print");

    CLI11_PARSE(app, argc, argv);

    cout << RULE << "\n DSS-Checker: Distributed State Space Checker\n" << RULE << "\n";
    cout << "Model             : " << file_name << "\n";
    if (!property_file.empty()) cout << "Properties        : " << property_file << "\n";
    cout << "Reduction         : " << (reduction ? "enabled" : "disabled") << "\n";
    if (!property_file.empty()) cout << "Fairness          : " << (fairness ? "enabled" : "disabled") << "\n";

    ConstructPetriFromFile construire;
    construire.setFileName(file_name);
    construire.setVerbose(verbose);

    cout << "\nModular Petri net\n";
    ModularPetriNet *petri = construire.getModularPetrinet();
    if (!petri) return 1;

    if (algorithm == "V1") {
        cout << "\nBuilding the distributed state space (V1)...\n";
        petri->buildDSS();
        cout << "Distributed state space built\n";
    }  else {
        cout << "\nBuilding the distributed state space...\n";
        DSSBuilder builder(petri, reduction);
        auto start = std::chrono::steady_clock::now();
        builder.build();
        auto finish = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
        cout << "DSS built in " << duration << " ms\n";

        cout << "\nDSS statistics\n";
        builder.printStats(cout);

        if (dot_output) {
            builder.writeToFile(file_name + ".dot");
            cout << "\nGraph saved in " << file_name << ".dot\n";
        }
        if (svg_output && builder.renderToFile(file_name + ".svg")) cout << "Graph rendered in " << file_name << ".svg\n";
        if (txt_output) {
            builder.outputTXT(file_name + ".txt");
            cout << "Statistics saved in " << file_name << ".txt\n";
        }
        if (property_file != "") {
            cout << "\n" << RULE << "\n LTL model checking\n" << RULE << "\n";
            bool dot_formula {false};
            size_t verified {}, violated {}, skipped {};
            double total_time {};
            // build automata of the negation of the formula
            Formula negate_formula;
            ParseFormulaFile parse_file{property_file};
            auto my_bdd_dict{spot::make_bdd_dict()};
            while (parse_file.getNext(negate_formula)) {
                // Module having all the atomic propositions: transitions, and places for the state-based ones
                const auto propositions {resolvePropositions(negate_formula.propositions)};
                if (!propositions) {
                    // Note: Spot reads a name starting with F, G or X (FF1a_1) as an operator applied to the rest
                    // (F(F1a_1)): such transition names must be double-quoted in the formula ("FF1a_1")
                    cout << "  Result            : SKIPPED\n";
                    cout << "  Reason            : no module has all the transitions and places of the formula:";
                    for (const auto &proposition: negate_formula.propositions) cout << " " << proposition;
                    cout << "\n  Hint              : names starting with F, G or X must be double-quoted\n";
                    ++skipped;
                    continue;
                }
                const auto index_module {propositions->module};
                cout << "  Module            : " << index_module << "\n";
                if (!propositions->predicates.empty()) {
                    cout << "  State propositions:";
                    for (const auto &predicate: propositions->predicates) cout << " [" << predicate.name << "]";
                    cout << "\n";
                }
                spot::twa_graph_ptr af{formula2Automaton(negate_formula.f, my_bdd_dict, dot_formula)};
                cout << "  Buchi automaton   : " << af->num_states() << " states (negation of the formula)\n";
                auto moduleGraph{builder.getModuleGraph(index_module)};

                auto k = std::make_shared<DSSKripke>(my_bdd_dict, moduleGraph, *propositions, fairness);
                auto start_intersect{clock()};
                auto res{(k->intersecting_run(af) == nullptr)};
                auto finish_intersect{clock()};
                auto duration_intersect {static_cast<double>(finish_intersect - start_intersect) / CLOCKS_PER_SEC};
                total_time += duration_intersect;
                if (res) ++verified; else ++violated;
                cout << "  Result            : " << (res ? "VERIFIED" : "VIOLATED") << "\n";
                cout << "  Verification time : " << duration_intersect << " s\n";
            }
            cout << "\n" << RULE << "\n";
            cout << "Summary           : " << verified + violated + skipped << " properties, " << verified
                 << " verified, " << violated << " violated, " << skipped << " skipped\n";
            cout << "Verification time : " << total_time << " s (total)\n";
            cout << RULE << "\n";
        }
    }
    if (dot_output && algorithm != "DSS") petri->writeToFile(file_name + ".dot");
    if (txt_output && algorithm != "DSS") petri->writeTextFile(file_name + ".txt");
    return 0;
}
