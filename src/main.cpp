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
#include <spot/ta/taproduct.hh>
#include <spot/twa/twaproduct.hh>
#include <spot/twaalgos/gtec/gtec.hh>
#include "DSS/DSSBuilder.h"


using namespace std;

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

    CLI11_PARSE(app, argc, argv);

    ConstructPetriFromFile construire;
    construire.setFileName(file_name);

    ModularPetriNet *petri;

    petri= construire.getModularPetrinet();
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
    } else if (algorithm == "V2") {
        cout << "Building the distributed state space (V2) has been started...\n";
        start = clock();
        dss = petri->buildReducedDSS();
        finish = clock();

    } else {
        cout << "Building DSS (new version) has been started...\n";
        DSSBuilder builder(petri);
        start = clock();
        builder.build();
        finish = clock();
        cout << "DSS has been successfully built." << endl;
    }

    //ModularSpace* espace_etat=petri->constructReducedStateSpace();

    duration = (double) (finish - start) / CLOCKS_PER_SEC;
    cout << duration << " seconds" << endl;

    if (dot_output) petri->writeToFile("fichier.dot");
    if (txt_output) petri->writeTextFile("fichier.txt");

    return 0;
    auto d = spot::make_bdd_dict();
}

