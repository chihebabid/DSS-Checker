// ConstructPetriFromFile.cpp: implementation of the ConstructPetriFromFile class.
//
//////////////////////////////////////////////////////////////////////


#include "ConstructPetriFromFile.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ConstructPetriFromFile::ConstructPetriFromFile() {
    m_file = NULL;
}

ConstructPetriFromFile::~ConstructPetriFromFile() {


}

void ConstructPetriFromFile::setFileName(const string name) {
    m_file_name = name;
}

string ConstructPetriFromFile::getFileName() {
    return m_file_name;
}

ModularPetriNet *ConstructPetriFromFile::getModularPetrinet() {
    ModularPetriNet *MPetri {nullptr};


    m_file = fopen(m_file_name.c_str(), "r");
    if (m_file) {
        //Cr�er le r�seau de Petri
        cout << "Loading modular Petri net from file..." << endl;
        MPetri = new ModularPetriNet;
        //Lecture de nombre de modules
        unsigned int nb_modules = 0;
        string word;
        getNextWord();
        word = getNextWord();
        nb_modules = atoi(word.c_str());
        cout << "This modular Petri net consists of " << word << " modules" << endl;
        cout << "Displaying informations related to the modular Petri net" << endl;
        //Initialisation
        PetriNet *petri;
        vector<Place> places;
        vector<Transition> transitions;
        for (unsigned int ii = 0; ii < nb_modules; ii++) {
            int transition_code = 0;
            cout << "Module n�" << (ii + 1) << endl;


            getNextWord();
            //
            // Lire les libell�s de places
            string str;
            petri = new PetriNet();
            // Sp�cifier le num du module
            petri->setNumber(ii);
            Place place;
            places.clear();
            /******************************************/
            /** Ajout des places et de leurs marquages*/
            /******************************************/
            do {
                int tokens = 00;
                word = getNextWord();
                while (word == "Marquage") {
                    word = getNextWord();
                    tokens = atoi(word.c_str());
//					printf("marquage :%d \n",marquage);

                    do {
                        word = getNextWord();
                        if (word != "Marquage" && word != "End") {
                            place.setName(word.c_str());
                            place.setTokens(tokens);
                            places.push_back(place);
                            cout << "Added place: " << word.c_str() << endl;
                        }
                    } while (word != "Marquage" && word != "End");
                }
            } while (word != "End");
            petri->addListPlaces(places);
            /****************************************/
            /** Lecture de libell�s de transitions **/
            /****************************************/
            getNextWord();
            Transition transition;
            transitions.clear();
            do {
                word = getNextWord();
                if (word != "End") {
                    transition.setName(word);
                    transition.setCode(transition_code);
                    transition_code++;
//					printf("Transition ajout�e :%s*\n",mot.c_str());
                    transitions.push_back(transition);
                }
            } while (word != "End");
            petri->addListTransitions(transitions);

            /****************************************/
            /** Lecture des entr�es de transitions **/
            /****************************************/

            getNextWord();
            vector<string> input_places;

            vector<int> weights;
            string transition_name;
            do {
                transition_name = getNextWord();
                if (transition_name != "End") {
                    cout << "Transition: " << transition_name.c_str() << " has as place inputs ";
                    string item;
                    while (item != "]") {
                        item = getNextWord();
                        if (item == "-") {
                            string weight;
                            weight = getNextWord();
                            int int_weight = atoi(weight.c_str());
                            weights.pop_back();
                            weights.push_back(int_weight);
                        } else if (item != "[" && item != "]") {
                            cout << item.c_str() << "<";
                            input_places.push_back(item);
                            weights.push_back(1);
                        }
                    }
                    if (input_places.size() == weights.size())
                        petri->addInputPlaces(transition_name, input_places, weights);
                    else cout << "Erreur liee au poids...";
                    input_places.clear();
                    weights.clear();
                }
                cout << endl;
            } while (transition_name != "End");



            /****************************************/
            /** Lecture des sorties de transitions **/
            /****************************************/
            getNextWord();
            vector<string> output_places;
            weights.clear();
            do {
                transition_name = getNextWord();
                if (transition_name != "End") {
                    cout << "Transition: " << transition_name.c_str() << " has as output places ";
                    string item;
                    while (item != "]") {
                        item = getNextWord();
                        if (item == "-") {
                            string weight;
                            weight = getNextWord();
                            int int_weight = atoi(weight.c_str());
                            weights.pop_back();
                            weights.push_back(int_weight);
                        } else if (item != "[" && item != "]") {
                            cout << ">" << item.c_str();
                            output_places.push_back(item);
                            weights.push_back(1);
                        }
                    }
                    if (output_places.size() == weights.size())
                        petri->addOutputPlaces(transition_name, output_places, weights);
                    else cout << "erreur liee au poids...";
                    output_places.clear();
                    weights.clear();
                }
                cout << endl;
            } while (transition_name != "End");
            MPetri->addModule(petri);
        }
        /*************************************************/
        /** Lecture de transititions de syn,chronisation**/
        /*************************************************/
        cout << "Determining synchronised transitions..." << endl;
        getNextWord();
        string transition;
        vector<string> sync_transitions;
        string temp;
        do {
            transition = getNextWord();
            if (transition != "End") {
                cout << transition.c_str() << " ";
                temp = transition;
                sync_transitions.push_back(temp);
            }

        } while (transition != "End");
        MPetri->setSync(sync_transitions);
    } else {
        cout << "Error: can't open file";
    }
    fclose(m_file);
    return MPetri;
}


string ConstructPetriFromFile::getNextWord() {
    string separators = ",;(:)";
    string str;
    char c = '\0';
    while (c <= (char) 32 || separators.find_first_of(c) != -1) {
        c = fgetc(m_file);
    }
    bool quit = false;
    while (c > 32 && !quit) {
        if (separators.find_first_of(c) != -1) {
            quit = true;
        } else {
            str += c;
            c = fgetc(m_file);
        }
    }
    return str;
}

