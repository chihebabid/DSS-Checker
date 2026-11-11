// PetriNet.cpp: implementation of the PetriNet class.
//
//////////////////////////////////////////////////////////////////////
#include <algorithm>
#include "Petri/PetriNet.h"
#include "misc.h"

typedef vector<element_t> Pile;
typedef vector<PElement> PStack;


//////////////////////////////////
// Renvoyer le nombre de places
//////////////////////////////////
int PetriNet::getPlaceCount() {
    return m_places.size();
}




//////////////////////////////////
// Ajouter une liste de places
//////////////////////////////////
void PetriNet::addListPlaces(vector<Place> places) {
    for (int i = 0; i < places.size(); i++)
        m_places.push_back(places[i]);
}

void PetriNet::addListTransitions(vector<Transition> transitions) {
    for (unsigned int i = 0; i < transitions.size(); i++) {
        transitions[i].setPetri(this->getNumber());
        m_transitions.push_back(transitions[i]);
    }
}


////////////////////////////////////////////
//  Retourner l'a&dresse d'une place
////////////////////////////////////////////
Place *PetriNet::getPlaceAddress(string placename) {
    int index = -1;
    for (int i = 0; i < m_places.size() && index == -1; i++)
        if (placename == m_places[i].getName()) index = i;
    if (index == -1) printf("Place not found!\n");
    return &m_places[index];
}

///////////////////////////////////////////////////////////////////////////////
// Retourner un pointeur sur une transition ayant le code pass� en param�tre //
///////////////////////////////////////////////////////////////////////////////
Transition *PetriNet::getTransitionAddress(const int code) {
    int index = -1;
    for (int i = 0; i < m_transitions.size() && index == -1; i++) {
        if (code == m_transitions[i].getCode()) index = i;
    }
    return &m_transitions[index];
}

Place PetriNet::getPlace(const int index) {
    return m_places[index];
}

////////////////////////////////
// Renvoyer le marquage courant
////////////////////////////////
Marking PetriNet::getMarking() {
    Marking marking {m_places.size()};
    for (int i = 0; i < m_places.size(); i++) {
        marking.add8BitsValue(m_places[i].getTokens(),i);
    }

    return marking;
}

void PetriNet::setMarking(Marking *marking) {

    for (int i = 0; i < m_places.size(); i++) {
        m_places[i].setTokens(marking->get8BitsValue(i));
    }
}

///////////////////////////////////////////////////////////////
// Renvoyer les libell�s des transitions franchissables
///////////////////////////////////////////////////////////////
vector<Transition *> PetriNet::getListFirableTransitions() {
    vector<Transition *> transitions;
    for (auto & transition : m_transitions)
        if (transition.isFirable()) {
            transitions.emplace_back(&transition);
        }

    return transitions;
}

void PetriNet::fire(Transition &t) {
    Transition *transition = getTransitionAddress(t.getCode());
    if (transition->isFirable()) transition->fire();
}


int PetriNet::addInputPlaces(string transition_name, vector<string> input_places, vector<int> weights) {
    // Localisation de l'indice de la transition
    int index = -1;
    for (int i = 0; i < m_transitions.size() && index == -1; i++) {
        if (m_transitions[i].getName() == transition_name)
            index = i;
    }
    if (index != -1) {
        // Ajout de places d'entr�e
        //printf("Places d'entr�e\n");
        for (int i = 0; i < input_places.size(); i++) {
            m_transitions[index].addInputPlace(getPlaceAddress(input_places[i]), weights[i]);
            //printf("%s, ",liste_places_entrees[i].mot.c_str());

        }
        return 0;
    }
    std::cerr << "\nError adding a relation...!\n";
    return -1;
}

/*
** Ajouter les places d'entres une transition
*/
int PetriNet::addOutputPlaces(string transition_name, vector<string> output_places, vector<int> weights) {
    // Localisation de l'indice de la transition
    int index = -1;
    for (int i = 0; i < m_transitions.size() && index == -1; i++) {
        if (m_transitions[i].getName() == transition_name)
            index = i;
    }
    if (index != -1) {
        // Ajout de places de sortie
        for (size_t j = 0; j < output_places.size(); ++j) {
            m_transitions[index].addOutputPlace(getPlaceAddress(output_places[j]), weights[j]);
            //printf("%s, ",liste_places_sorties[j].mot.c_str());
        }
        return 0;
    }
    std::cerr << "ERROR : wrong relation!\n";;
    return -1;
}

/////////////////////////////////////
/// Renvoyer le nom d'un marquage ///
/////////////////////////////////////
string PetriNet::getMarkingName(Marking marking) {
    string result;
    char str[100] = "\0";
    for (int j = 0; j < getPlaceCount(); j++) {
        if (marking.get8BitsValue(j) != 0) {
            if (marking.get8BitsValue(j) != 1) {
                sprintf(str, "%d", (int) marking.get8BitsValue(j));
                result += str;
            }
            result += getPlace(j).getName();
            result += "_";
        }
    }
    return result;
}


///////////////////////////////////
// Sp�cifier le num�ro du module //
///////////////////////////////////
void PetriNet::setNumber(const int index) {
    m_number = index;
}

/////////////////////////////////////
// Renvoyer le num�ro de ce module //
/////////////////////////////////////
int PetriNet::getNumber() {
    return m_number;
}

/////////////////////////////////////////////////////////////////////
// Renvoyer un pointeur vers une transition en connaissant son nom //
/////////////////////////////////////////////////////////////////////
Transition *PetriNet::getTransitionAddress(const string transition_name) {

    for (int i = 0; i < m_transitions.size(); i++) {
        if (transition_name == m_transitions[i].getName())  return &m_transitions[i];
    }
    return nullptr;
}





// Construction de Meta-graphe

MetaState *PetriNet::getMetaState(Marking marking) {
    //vector<Marquage> list_marq_inserted;
    PStack pstack;
    PElement elt;
    auto *ms = new MetaState();
    elt.marking = new Marking(marking);
    setMarking(&marking);
    elt.transitions = getListFirableTransitions();
    pstack.push_back(elt);


    ms->insertMarking(elt.marking);

    while (pstack.size() > 0) {
        PElement current_elt = pstack.back();
        pstack.pop_back();

        while (current_elt.transitions.size() > 0) {

            Transition *transition = current_elt.transitions.back();
            current_elt.transitions.pop_back();

            // On doit tirer la transition correspondante apr�s avoir pr�ciser le marquage
            setMarking(current_elt.marking);
            //cout<<"\n The old  marquage="<<getMarkingName(getMarking())<<endl;
            fire(*transition); // Franchissement

            Marking *new_state;
            new_state = new Marking(getMarking());
            // Indiquer si le marquage en question est récemment inséré
            //if (!state_graph->existMarking(new_state)) list_marq_inserted.push_back(*new_state);
            //cout<<"\n The old  marquage="<<current_elt.marking.get8BitsValue()<<endl;
            //cout<<"firing of "<<(*transition).getName()<<endl;
            //cout<<"\n The new  marquage="<<getMarkingName(*new_state)<<endl;
            Marking *old_state = ms->insertMarking(new_state);
            //if (old_state) cout<<"\n The found  marquage="<<getMarkingName(*old_state)<<endl;

            //cout<<"\n The current  marquage (old)="<<getMarkingName(*current_elt.marking)<<endl;
            if (!old_state) current_elt.marking->addSucc(transition, new_state);
            else {
                delete new_state;
                new_state = nullptr;
                current_elt.marking->addSucc(transition, old_state);

            }


            //cout<<"\n Ajout du noeud réusssi ="<<result;
            //if (current_elt.transitions.size()>0) pstack.push_back(current_elt);
            // Ajouter le marquage dans la pile et ses transitions franchissables
            if (!old_state) {
                elt.marking = new_state;
                setMarking(elt.marking);
                elt.transitions = getListFirableTransitions();
                if (elt.transitions.size() > 0) pstack.push_back(elt);
            }
        }
    }

    ms->computeSCCTarjan();
    return ms;
}


void PetriNet::printMetaStateEx(MetaState *ms) {
    auto list_states = ms->getListMarkings();

    cout << "Number of states: " << list_states.size() << endl;
    cout << "Number of arcs: " << ms->getArcCount() << endl;
    //for (ms->
    /*for (int i=0;i<list_arcs->size();i++) {
        cout<<"Arc n°"<<i<<": ("<<getMarkingName(*list_arcs->at(i).getSource())<<" ===";
        cout<<list_arcs->at(i).getTransition()->getName()<<"==> ";
        cout<<getMarkingName(*list_arcs->at(i).getDestination())<<")"<<endl;
    }
    vector<SCC*>* list_sccs=ms->getListSCCs();
    cout<<"Number of SCCs: "<<list_sccs->size()<<endl;
    for (int i=0;i<list_sccs->size();i++) {
        cout<<"SCC n°"<<i<<endl;
        vector<Marking*>* lscc_states=list_sccs->at(i)->getListStates();
        for (int j=0;j<lscc_states->size();j++) {
            cout<<getMarkingName(*lscc_states->at(j))<<" , ";
        }
        cout<<endl;
    }*/
}


string PetriNet::getSCCName(SCC *scc) {
    return getMarkingName(*scc->getListStates()->at(0));
}

bool PetriNet::areTransitionsIncluded(const std::set<string> &list_transitions) {
    for (const auto &transition: list_transitions) {
        if (std::find_if(m_transitions.begin(), m_transitions.end(), [transition](auto &elt) { return elt.getName() == transition; }) == m_transitions.end()) {
            return false;
        }
    }
    return true;
}
