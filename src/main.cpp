#include <iostream>
#include "ConstructPetriFromFile.h"
#include "time.h"
#include <sys/wait.h>
#include <unistd.h>
#include <spot/misc/version.hh>
#include <spot/twaalgos/dot.hh>
#include <spot/tl/parse.hh>
#include <spot/tl/print.hh>
#include <spot/twaalgos/translate.hh>
#include <spot/twaalgos/emptiness.hh>
#include <spot/tl/apcollect.hh>
#include <spot/ta/taproduct.hh>
#include <spot/twa/twaproduct.hh>
#include <spot/twaalgos/gtec/gtec.hh>



using namespace std;

int main(int argc, char* argv[]) {

	char file_name[100]; 
	CConstructPetriFromFile construire;
	if (argc == 1) {
		cout << "Name of file is not specified!" << endl;
		cout << "Please enter a name of file:" << endl;
		cin>>file_name;
		construire.setFileName(file_name);
	}
	else construire.setFileName(argv[1]);

	CModularPetriNet* petri = construire.getModularPetrinet();
	cout << "Petri net information loaded successfully...\n";
	cout << "Petri net built from file : " << construire.getFileName()<<endl;

	vectorString liste_transitions;
	if (argc > 2) {
		for (int i = 0; i < argc - 2; i++) {
			liste_transitions.push_back((string) argv[i + 2]);
		}
	}
	//petri->renommerTransitions(liste_transitions);
	unsigned int choix;
	do {
		cout << endl << endl
				<< "###==================== MENU ================###" << endl
				<< endl << endl;
		cout << "1. Build the distributed state space" << endl;
		cout << "2. Build the reduced distributed state space" << endl << endl;
		cout << "X. Exit" << endl;
		cout << "Enter your choice : ";
		cin >> choix;
	} while (choix > 3 || choix < 0);

	clock_t start, finish;
	double duration;

	// DistributedState dss(petri);
	DistributedState* dss;

	switch (choix) {
	case 1:
		cout << "Building the distributed state space has been started..."
				<< endl;
		start = clock();
		dss = petri->buildDSS();
		finish = clock();
		cout << "Distributed state space successfully built." << endl;
		break;
	case 2:
		start = clock();
		dss = petri->buildReducedDSS();
		finish = clock();
		break;
	}

	//ModularSpace* espace_etat=petri->constructReducedStateSpace();

	duration = (double) (finish - start) / CLOCKS_PER_SEC;
	cout << duration << " seconds" << endl;

	do {
		cout << "\n\n=============== MENU ===============\n" << endl;
		cout << "1. Write dot file" << endl;
		cout << "2. Write text file" << endl;
		cout << "Enter your choice :";
		cin >> choix;
	} while (choix == 0 || choix > 3);
	switch (choix) {
	case 1:
		petri->writeToFile("fichier.dot");
		break;
	case 2:
		petri->writeTextFile("fichier.txt");
		break;
	}
	return 0;
    auto d = spot::make_bdd_dict();
}

