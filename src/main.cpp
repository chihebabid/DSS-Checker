#include <iostream>
#include "ConstructPetriFromFile.h"
#include "time.h"
#include <sys/wait.h>
#include <unistd.h>
#include <spot/misc/version.hh>
#include <spot/twaalgos/dot.hh>
#include <CLI11.hpp>
#include <spot/tl/parse.hh>
#include <spot/tl/print.hh>
#include <spot/twaalgos/translate.hh>
#include <spot/twaalgos/emptiness.hh>
#include <spot/tl/apcollect.hh>
#include "DSS/DSSBuilder.h"
#include "SpotMC/DSSKripke.h"
#include "SpotMC/ParseFormulaFile.h"
using namespace std;


/**
 * Parse a file  containing a LTL formula and return its negation
 *
 * @param fileName Path to LTL formula
 * @return Formula
 */
Formula negateFormula(const string &fileName) {
    string input;
    set<string> transitionSet;
    ifstream file(fileName);

    cout << "______________________________________" << endl;
    cout << "Fetching formula ... " << endl;

    if (!file.is_open()) {
        cout << "Cannot open formula file" << endl;
        exit(0);
    }

    getline(file, input);
    cout << "        Loaded formula: " << input << endl;
    file.close();

    spot::parsed_formula pf = spot::parse_infix_psl(input);
    if (pf.format_errors(std::cerr))
        exit(0);

    spot::formula fo = pf.f;
    if (!fo.is_ltl_formula()) {
        std::cerr << "Only LTL formulas are supported.\n";
        exit(0);
    }

    spot::atomic_prop_set *p_list = spot::atomic_prop_collect(fo, nullptr);
    for (const auto &i: *p_list) {
        transitionSet.insert(i.ap_name());
    }
    cout << "Formula in SPIN format: ";
    print_spin_ltl(std::cout, fo) << endl;

    cout << "Building negation of the formula ... ";
    spot::formula not_f = spot::formula::Not(pf.f);
    cout << "done\n"
         << endl;

    return {not_f, transitionSet};
}

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

    string file_name = "";
    app.add_option("--file", file_name, "Petri net model file")
            ->type_name("Path")
            ->required()
            ->check(CLI::ExistingFile);

    string algorithm = "DSS";
    app.add_option("--algorithm", algorithm, "DSS building algorithm (default: DSS)")->group("Algorithm");

    bool dot_output{false};
    app.add_flag("--dot-output", dot_output, "Save the output graph in a dot file")->group("Print");

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

    vectorString liste_transitions;
    if (argc > 2) {
        for (int i = 0; i < argc - 2; i++) {
            liste_transitions.push_back((string) argv[i + 2]);
        }
    }
    //petri->renommerTransitions(liste_transitions);


    clock_t start, finish;
    double duration;

    // DistributedState dss(petri);
    DistributedState *dss;


    if (algorithm == "V1") {
        cout << "Building the distributed state space (V1) has been started..."
             << endl;
        start = clock();
        dss = petri->buildDSS();
        finish = clock();
        cout << "Distributed state space successfully built." << endl;
    }  else {
        cout << "Building DSS (new version) has been started...\n";
        DSSBuilder builder(petri);
        start = clock();
        builder.build();
        finish = clock();
        duration = (double) (finish - start) / CLOCKS_PER_SEC;
        cout << "DSS has been successfully built." << endl;
        if (dot_output) builder.writeToFile(file_name + ".dot");
        if (txt_output) builder.outputTXT();
        if (property_file != "") {
            bool dot_formula {false};
            // build automata of the negation of the formula
            Formula negate_formula {negateFormula(property_file)};
            ParseFormulaFile parse_file{property_file};
            while (parse_file.getNext(negate_formula)) {
                auto my_bdd_dict{spot::make_bdd_dict()};
                spot::twa_graph_ptr af{formula2Automaton(negate_formula.f, my_bdd_dict, dot_formula)};

                // Get module index
                auto index_module{petri->getModule(negate_formula.propositions)};
                std::cout << "\tProperty will be checked against module #" << index_module << std::endl;
                auto moduleGraph{builder.getModuleGraph(index_module)};
                // build a twa graph of the SOG

                auto k = std::make_shared<DSSKripke>(my_bdd_dict, moduleGraph);
                auto start_interset{clock()};
                auto res{(k->intersecting_run(af) == 0)};
                auto finish_intersect{clock()};
                if (res)
                    cout << "\tProperty is verified...\n";
                else
                    cout << "\tProperty is violated...\n";
                auto duration_intersect {(double) (finish - start) / CLOCKS_PER_SEC};
                cout<<"\tVerification duration: "<<duration_intersect<<'\n';
            }
        }
    }

    cout<<"\nDSS building duration: "<< duration << " seconds" << endl;

    if (dot_output && algorithm != "DSS") petri->writeToFile(file_name + ".dot");
    if (txt_output && algorithm != "DSS") petri->writeTextFile(file_name + ".txt");

    return 0;
}