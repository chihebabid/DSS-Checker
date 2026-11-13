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
    cout << "\tBuilding automata for not(formula) ... ";
    spot::translator tmp = spot::translator(bdd);
    tmp.set_type(spot::postprocessor::BA);
    spot::twa_graph_ptr af = tmp.run(f);
//    spot::twa_graph_ptr af = spot::translator(bdd).run(f);
    cout << "done\n";

    // save the generated automaton in a dot file
    if (save_dot) {
        saveGraph(af, "negated_formula.dot");
    }

    return af;
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
    app.add_flag("--txt-output", txt_output, "Save the output graph in a formatted text file")->group("Print");


    string property_file = "";
    app.add_option("--mc", property_file, "LTL property file")
            ->type_name("Path")
            ->check(CLI::ExistingFile);

    CLI11_PARSE(app, argc, argv);

    ConstructPetriFromFile construire;
    construire.setFileName(file_name);

    ModularPetriNet *petri;

    petri = construire.getModularPetrinet();
    cout << "Petri net information loaded successfully...\n";
    cout << "Petri net built from file : " << construire.getFileName() << endl;

    if (algorithm == "V1") {
        cout << "Building the distributed state space (V1) has been started..."
             << endl;
        petri->buildDSS();
        cout << "Distributed state space successfully built." << endl;
    }  else {
        cout << "Building DSS (new version) has been started...\n";
        DSSBuilder builder(petri, reduction);
        auto start = std::chrono::steady_clock::now();//clock();
        builder.build();
        auto finish = std::chrono::steady_clock::now();
        //finish = clock();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
        cout<<"\nDSS building duration: "<< duration << " ms" << endl;
        if (dot_output) builder.writeToFile(file_name + ".dot");
        if (svg_output && builder.renderToFile(file_name + ".svg")) cout << "Graph rendered in " << file_name << ".svg\n";
        if (txt_output) builder.outputTXT();
        if (property_file != "") {
            bool dot_formula {false};
            // build automata of the negation of the formula
            Formula negate_formula;
            ParseFormulaFile parse_file{property_file};
            auto my_bdd_dict{spot::make_bdd_dict()};
            while (parse_file.getNext(negate_formula)) {

                spot::twa_graph_ptr af{formula2Automaton(negate_formula.f, my_bdd_dict, dot_formula)};

                // Get module index
                auto index_module{petri->getModule(negate_formula.propositions)};
                std::cout << "\tProperty will be checked against module #" << index_module << std::endl;
                auto moduleGraph{builder.getModuleGraph(index_module)};
                // build a twa graph of the SOG

                auto k = std::make_shared<DSSKripke>(my_bdd_dict, moduleGraph, negate_formula.propositions, fairness);
                auto start_intersect{clock()};
                auto res{(k->intersecting_run(af) == nullptr)};
                auto finish_intersect{clock()};
                if (res)
                    cout << "\tProperty is verified...\n";
                else
                    cout << "\tProperty is violated...\n";
                auto duration_intersect {static_cast<double>(finish_intersect - start_intersect) / CLOCKS_PER_SEC};
                cout<<"\tVerification duration: "<<duration_intersect<<'\n';
            }
        }
    }
    //cout<<"\nDSS building duration: "<< duration << " seconds" << endl;
    if (dot_output && algorithm != "DSS") petri->writeToFile(file_name + ".dot");
    if (txt_output && algorithm != "DSS") petri->writeTextFile(file_name + ".txt");
    return 0;
}