// PetriNet.cpp: implementation of the PetriNet class.
//
//////////////////////////////////////////////////////////////////////
#include <algorithm>
#include "Automata.h"
#include "PetriNet.h"
#include "misc.h"

typedef vector<element_t> Pile;
typedef vector<PElement> PStack;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
PetriNet::PetriNet() {
    m_places.clear();
    ml_transitions.clear();
    m_graphe = NULL;
}

PetriNet::~PetriNet() {

}

//////////////////////////////////
// Renvoyer le nombre de places
//////////////////////////////////
int PetriNet::getPlacesCount() {
    return m_places.size();
}




//////////////////////////////////
// Ajouter une liste de places
//////////////////////////////////
void PetriNet::addListPlaces(vector<Place> liste_places) {
    for (int i = 0; i < liste_places.size(); i++)
        m_places.push_back(liste_places[i]);
}

void PetriNet::addListTransitions(vector<Transition> liste_transitions) {
    for (unsigned int i = 0; i < liste_transitions.size(); i++) {
        liste_transitions[i].setPetri(this->getNumero());
        ml_transitions.push_back(liste_transitions[i]);
    }
}


////////////////////////////////////////////
//  Retourner l'a&dresse d'une place
////////////////////////////////////////////
Place *PetriNet::getPlaceAdresse(string placename) {
    int indice = -1;
    for (int i = 0; i < m_places.size() && indice == -1; i++)
        if (placename == m_places[i].getName()) indice = i;
    if (indice == -1) printf("Place introuvable : libell� d'une place in�xistante!\n");
    return &m_places[indice];
}

///////////////////////////////////////////////////////////////////////////////
// Retourner un pointeur sur une transition ayant le code pass� en param�tre //
///////////////////////////////////////////////////////////////////////////////
Transition *PetriNet::getTransitionAdresse(const int code) {
    int indice = -1;
    for (int i = 0; i < ml_transitions.size() && indice == -1; i++) {
        if (code == ml_transitions[i].getCode()) indice = i;
    }
    return &ml_transitions[indice];
}

Place PetriNet::getPlace(const int index) {
    return m_places[index];
}

////////////////////////////////
// Renvoyer le marquage courant
////////////////////////////////
Marking PetriNet::getMarquage() {
    Marking vecteur;
    for (int i = 0; i < m_places.size(); i++) {
        vecteur.add8BitsValue(m_places[i].getTokens());
    }

    return vecteur;
}

void PetriNet::setMarquage(Marking *marquage) {

    for (int i = 0; i < m_places.size(); i++) {
        m_places[i].setTokens(marquage->get8BitsValue(i));
    }
}

///////////////////////////////////////////////////////////////
// Renvoyer les libell�s des transitions franchissables
///////////////////////////////////////////////////////////////
vector<Transition *> PetriNet::getListeTransitionsFranchissables() {
    vector<Transition *> liste_transitions;
    for (int i = 0; i < ml_transitions.size(); i++)
        if (ml_transitions.at(i).isFranchissable()) {
            liste_transitions.push_back(&ml_transitions.at(i));
        }

    return liste_transitions;
}

void PetriNet::tirer(Transition &t) {
    Transition *transition = getTransitionAdresse(t.getCode());
    if (transition->isFranchissable()) transition->tirer();
}


int PetriNet::addPlacesEntrees(string nom_transition, vector<string> liste_places_entrees, vector<int> liste_poids) {
    // Localisation de l'indice de la transition
    int indice = -1;
    for (int i = 0; i < ml_transitions.size() && indice == -1; i++) {
        if (ml_transitions[i].getName() == nom_transition)
            indice = i;
    }
    if (indice != -1) {
        // Ajout de places d'entr�e
        //printf("Places d'entr�e\n");
        for (int i = 0; i < liste_places_entrees.size(); i++) {
            ml_transitions[indice].addPlaceEntree(getPlaceAdresse(liste_places_entrees[i]), liste_poids.at(i));
            //printf("%s, ",liste_places_entrees[i].mot.c_str());

        }
        return 0;
    }
    cout << "\nErreur d'ajout d'une relation!\n";
    return -1;
}

///////////////////////////////////////////////////////////
// Ajouter les places d'entr�es � une transition
///////////////////////////////////////////////////////////
int PetriNet::addPlacesSorties(string nom_transition, vector<string> liste_places_sorties, vector<int> liste_poids) {
    // Localisation de l'indice de la transition
    int indice = -1;
    for (int i = 0; i < ml_transitions.size() && indice == -1; i++) {
        if (ml_transitions[i].getName() == nom_transition)
            indice = i;
    }
    if (indice != -1) {
        // Ajout de places de sortie
        for (int j = 0; j < liste_places_sorties.size(); j++) {
            ml_transitions[indice].addPlaceSortie(getPlaceAdresse(liste_places_sorties.at(j)), liste_poids.at(j));
            //printf("%s, ",liste_places_sorties[j].mot.c_str());
        }
        return 0;
    }
    printf("ERREUR : relation erron�e!\n");
    return -1;
}

/////////////////////////////////////
/// Renvoyer le nom d'un marquage ///
/////////////////////////////////////
string PetriNet::getMarquageName(Marking marquage) {
    string resultat = "";
    char chaine[100] = "\0";
    for (int j = 0; j < getPlacesCount(); j++) {
        if (marquage.get8BitsValue(j) != 0) {
            if (marquage.get8BitsValue(j) != 1) {
                sprintf(chaine, "%d", (int) marquage.get8BitsValue(j));
                resultat += chaine;
            }
            resultat += getPlace(j).getName();
            resultat += "_";
        }
    }
    return resultat;
}


///////////////////////////////////
// Sp�cifier le num�ro du module //
///////////////////////////////////
void PetriNet::setNumero(const int index) {
    m_numero = index;
}

/////////////////////////////////////
// Renvoyer le num�ro de ce module //
/////////////////////////////////////
int PetriNet::getNumero() {
    return m_numero;
}

/////////////////////////////////////////////////////////////////////
// Renvoyer un pointeur vers une transition en connaissant son nom //
/////////////////////////////////////////////////////////////////////
Transition *PetriNet::getTransitionAdresse(const string nom_transition) {
    int indice = -1;
    for (int i = 0; i < ml_transitions.size() && indice == -1; i++) {
        if (nom_transition == ml_transitions[i].getName()) indice = i;
    }
    if (indice != -1)
        return &ml_transitions[indice];
    else return NULL;
}



////////////////////////////////////////////////////////////
// Mettre � jour la liste des fils
////////////////////////////////////////////////////////////
void PetriNet::update(vector<Fils> &liste_fils, Marking marq, Transition &transition) {
    int indice = -1;
    for (int i = 0; i < liste_fils.size() && (indice == -1); i++) {
        if (liste_fils.at(i).getMarquage() == marq) indice = i;
    }
    if (indice != -1) {
        liste_fils.at(indice).addTransition(transition);
    } else {
        Fils fils;
        fils.setMarquage(marq);
        fils.addTransition(transition);
        liste_fils.push_back(fils);
    }
}

bool PetriNet::isDivergent(Marking &marq) {
    Noeud *noeud = m_graphe->getFirstNodeContainMarquage(&marq);
    if (noeud == NULL) return false;
    vector<Transition *> liste_transitions;
    setMarquage(&marq);
    liste_transitions = getListeTransitionsFranchissables();
    for (int t = 0; t < liste_transitions.size(); t++) {
        setMarquage(&marq);
        tirer(*liste_transitions.at(t));
        Marking m = getMarquage();
        if (noeud->existMarquage(&m)) return true;
    }
    return false;

}



// Construction de Meta-graphe

StateGraph *PetriNet::getStateGraph(Marking marquage) {
    //vector<Marquage> list_marq_inserted;
    PStack pstack;
    PElement elt;
    StateGraph *state_graph = new StateGraph();
    elt.marquage = new Marking(marquage);
    setMarquage(&marquage);
    elt.liste_transitions = getListeTransitionsFranchissables();
    pstack.push_back(elt);

    // ListMarquage* node=new ListMarquage();
    // node->addMarquage(&marquage);
    // Indiquer si le marquage en question est r�cemment ins�r�
    //if (!state_graph->existState(&marquage)) list_marq_inserted.push_back(marquage);
    state_graph->addMarquage(elt.marquage);

    while (pstack.size() > 0) {
        PElement current_elt = pstack.back();
        pstack.pop_back();

        while (current_elt.liste_transitions.size() > 0) {

            Transition *transition = current_elt.liste_transitions.back();
            current_elt.liste_transitions.pop_back();

            // On doit tirer la transition correspondante apr�s avoir pr�ciser le marquage
            setMarquage(current_elt.marquage);
            //cout<<"\n The old  marquage="<<getMarquageName(getMarquage())<<endl;
            tirer(*transition); // Franchissement

            Marking *new_state;
            new_state = new Marking(getMarquage());
            // Indiquer si le marquage en question est r�cemment inséré
            //if (!state_graph->existState(new_state)) list_marq_inserted.push_back(*new_state);
            //cout<<"\n The old  marquage="<<current_elt.marquage.get8BitsValue()<<endl;
            //cout<<"firing of "<<(*transition).getName()<<endl;
            //cout<<"\n The new  marquage="<<getMarquageName(*new_state)<<endl;
            Marking *old_state = state_graph->addMarquage(new_state);
            //if (old_state) cout<<"\n The found  marquage="<<getMarquageName(*old_state)<<endl;

            //cout<<"\n The current  marquage (old)="<<getMarquageName(*current_elt.marquage)<<endl;
            if (!old_state) current_elt.marquage->addSucc(transition, new_state);
            else {
                delete new_state;
                new_state = NULL;
                current_elt.marquage->addSucc(transition, old_state);

            }


            //cout<<"\n Ajout du noeud réusssi ="<<result;
            //if (current_elt.liste_transitions.size()>0) pstack.push_back(current_elt);
            // Ajouter le marquage dans la pile et ses transitions franchissables
            if (!old_state) {
                elt.marquage = new_state;
                setMarquage(elt.marquage);
                elt.liste_transitions = getListeTransitionsFranchissables();
                if (elt.liste_transitions.size() > 0) pstack.push_back(elt);
            }
        }
    }

    state_graph->computeSCCTarjan();
    return state_graph;
}


void PetriNet::printMetaStateEx(MetaState *ms) {
    vector<Marking *> *list_states = ms->getListMarq();

    cout << "Number of states: " << list_states->size() << endl;
    cout << "Number of arcs: " << ms->getArcCount() << endl;
    //for (ms->
    /*for (int i=0;i<list_arcs->size();i++) {
        cout<<"Arc n°"<<i<<": ("<<getMarquageName(*list_arcs->at(i).getSource())<<" ===";
        cout<<list_arcs->at(i).getTransition()->getName()<<"==> ";
        cout<<getMarquageName(*list_arcs->at(i).getDestination())<<")"<<endl;
    }
    vector<SCC*>* list_sccs=ms->getListSCCs();
    cout<<"Number of SCCs: "<<list_sccs->size()<<endl;
    for (int i=0;i<list_sccs->size();i++) {
        cout<<"SCC n°"<<i<<endl;
        vector<Marking*>* lscc_states=list_sccs->at(i)->getListStates();
        for (int j=0;j<lscc_states->size();j++) {
            cout<<getMarquageName(*lscc_states->at(j))<<" , ";
        }
        cout<<endl;
    }*/
}


string PetriNet::getSCCName(SCC *scc) {
    return getMarquageName(*scc->getListStates()->at(0));
}

bool PetriNet::areTransitionsIncluded(const std::set<string> &list_transitions) {
    for (const auto &transition: list_transitions) {
        if (std::find_if(ml_transitions.begin(), ml_transitions.end(), [transition](auto &elt) { return elt.getName() == transition; }) == ml_transitions.end()) {
            return false;
        }
    }
    return true;
}
