// PetriNet.cpp: implementation of the PetriNet class.
//
//////////////////////////////////////////////////////////////////////
#include <iostream>
#include <algorithm>
#include <cctype>
#include <map>
#include "Petri/PetriNet.h"
#include "DSS/MetaState.h"
#include "DSS/SCC.h"
#include "misc.h"



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
        marking.setTokens(i, m_places[i].getTokens());
    }

    return marking;
}

void PetriNet::setMarking(const Marking *marking) {

    for (int i = 0; i < m_places.size(); i++) {
        m_places[i].setTokens(marking->getTokens(i));
    }
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
string PetriNet::getMarkingName(const Marking &marking) {
    string result;
    char str[100] = "\0";
    for (int j = 0; j < getPlaceCount(); j++) {
        if (marking.getTokens(j) != 0) {
            if (marking.getTokens(j) != 1) {
                sprintf(str, "%d", (int) marking.getTokens(j));
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

void PetriNet::buildLocalTransitions() {
    m_local_transitions.clear();
    for (auto &transition: m_transitions) {
        if (transition.isSync()) continue; // a synchronised transition is never fired locally
        LocalTransition local {&transition, {}, {}};
        std::map<uint32_t, int> delta;
        for (const auto &[place, weight]: transition.getInputPlaces()) {
            const auto index {static_cast<uint32_t>(place - m_places.data())};
            local.inputs.emplace_back(index, weight);
            delta[index] -= weight;
        }
        for (const auto &[place, weight]: transition.getOutputPlaces()) {
            delta[static_cast<uint32_t>(place - m_places.data())] += weight;
        }
        for (const auto &[index, d]: delta) {
            if (d != 0) local.deltas.emplace_back(index, d);
        }
        m_local_transitions.push_back(std::move(local));
    }
    m_local_transitions_built = true;
}

PetriNet::~PetriNet() = default;

MetaState *PetriNet::getMetaState(const Marking &marking) {
    return new MetaState(getLocalGraph(marking));
}

SCC *PetriNet::findComputedSCC(const Marking &marking) const {
    const auto it {m_scc_index.find(&marking)};
    return it == m_scc_index.end() ? nullptr : it->second;
}

LocalGraph *PetriNet::getLocalGraph(const Marking &marking) {
    // marking in the initial SCC of a graph: this graph is the set of the markings reached from it
    if (SCC *scc {findComputedSCC(marking)}; scc && scc == scc->getParent()->getInitialSCC()) return scc->getParent();
    std::unique_ptr<LocalGraph> graph {exploreLocalGraph(marking)};
    const size_t hash {graph->contentHash()};
    const auto [first, last] {m_graph_index.equal_range(hash)};
    for (auto it {first}; it != last; ++it) {
        if (*it->second == *graph) return it->second; // The new graph is destroyed
    }
    LocalGraph *stored {graph.get()};
    m_graphs.push_back(std::move(graph));
    m_graph_index.emplace(hash, stored);
    for (const auto scc: stored->getListSCCs()) {
        for (const auto m: *scc->getListStates()) m_scc_index.emplace(m, scc);
    }
    return stored;
}

/*
 * Local graph reached from marking by local transitions (depth-first exploration).
 * Markings are handled directly (no Place object involved, the current marking of the net is not
 * modified); a new Marking is only allocated for a new state. The markings, their successors and the
 * SCCs are produced in the same order as before.
 */
std::unique_ptr<LocalGraph> PetriNet::exploreLocalGraph(const Marking &marking) {
    if (!m_local_transitions_built) buildLocalTransitions();
    // Local transitions enabled at m, in the order of m_transitions
    const auto enabled = [this](const Marking &m) {
        vector<uint32_t> result;
        for (uint32_t i {}; i < m_local_transitions.size(); ++i) {
            const auto &inputs {m_local_transitions[i].inputs};
            if (std::all_of(inputs.begin(), inputs.end(), [&m](const auto &input) {
                return m.getTokens(input.first) >= input.second;
            })) {
                result.push_back(i);
            }
        }
        return result;
    };

    auto graph {std::make_unique<LocalGraph>()};
    auto *initial = new Marking(marking);
    graph->insertMarking(initial);
    struct Element {
        Marking *marking;
        vector<uint32_t> transitions; // Enabled transitions not fired yet (the last one is fired first)
    };
    vector<Element> stack;
    stack.push_back({initial, enabled(*initial)});
    Marking reached {marking.getPlaceCount()}; // Reused for every firing

    while (!stack.empty()) {
        Element current {std::move(stack.back())};
        stack.pop_back();
        while (!current.transitions.empty()) {
            const LocalTransition &local {m_local_transitions[current.transitions.back()]};
            current.transitions.pop_back();
            // Firing
            reached = *current.marking;
            for (const auto &[place, delta]: local.deltas) {
                reached.setTokens(place, static_cast<uint32_t>(static_cast<int>(reached.getTokens(place)) + delta));
            }
            if (Marking *old_state {graph->existMarking(&reached)}; old_state) {
                current.marking->addSucc(local.transition, old_state);
                continue;
            }
            // Only allocate a node if the reached marking is a new one
            auto *new_state = new Marking(reached);
            graph->insertMarking(new_state);
            current.marking->addSucc(local.transition, new_state);
            if (auto transitions {enabled(*new_state)}; !transitions.empty()) {
                stack.push_back({new_state, std::move(transitions)});
            }
        }
    }

    graph->computeSCCTarjan();
    return graph;
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
    const vector<SCC*>* list_sccs=ms->getListSCCs();
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


// The name of an SCC is the name of its representative marking (its first one), in upper case
string PetriNet::getSCCName(SCC *scc) {
    string name {getMarkingName(*scc->getListStates()->at(0))};
    std::transform(name.begin(), name.end(), name.begin(), [](const unsigned char c) { return std::toupper(c); });
    return name;
}

bool PetriNet::areTransitionsIncluded(const std::set<string> &list_transitions) {
    for (const auto &transition: list_transitions) {
        if (std::find_if(m_transitions.begin(), m_transitions.end(), [transition](auto &elt) { return elt.getName() == transition; }) == m_transitions.end()) {
            return false;
        }
    }
    return true;
}
