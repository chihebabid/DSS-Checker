// PetriNet.h: interface for the PetriNet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PETRINET_H__91DDDBD6_8669_4EC2_A0F5_E20F2FAFDC37__INCLUDED_)
#define AFX_PETRINET_H__91DDDBD6_8669_4EC2_A0F5_E20F2FAFDC37__INCLUDED_

#include <set>
#include "Marking.h"
#include "Fils.h"
#include "StateGraph.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class PilePhase1;

class PileRed;

class Transition;

class Automata;

class GrapheSync;

class Fils;

class ListMarquage;

class Graphe;

class StateGraph;

class MetaState;

class SCC;

using std::vector;
typedef vector<string> vectorString;

class PetriNet {
public:
    string getSCCName(SCC *scc);

    bool isDivergent(Marking &marq);

    int addPlacesEntrees(string nom_transition, vector<string> liste_places_entrees, vector<int> liste_poids);

    Transition *getTransitionAdresse(const string nom_transition);

    int getNumero();

    void setNumero(const int index);

    string getMarquageName(Marking marquage);

    int addPlacesSorties(string nom_transition, vector<string> liste_places_sorties, vector<int> liste_poids);

    void tirer(Transition &t);

    void setMarquage(Marking *marquage);

    Marking getMarquage();

    Place getPlace(const int index);

    Transition *getTransitionAdresse(const int code);

    void addListTransitions(vector<Transition> liste_transitions);

    void addListPlaces(vector<Place> liste_places);

    int getPlacesCount();

    PetriNet();

    virtual ~PetriNet();

    StateGraph *getStateGraph(Marking marquage);



    void printMetaStateEx(MetaState *ms);

    bool areTransitionsIncluded(const std::set<string> &list_transitions);
private:
    Automata *m_graphe;

    void update(vector<Fils> &liste_fils, Marking marq, Transition &transition);

    int m_numero;

    vector<Transition *> getListeTransitionsFranchissables();

    Place *getPlaceAdresse(string placename);

    vector<Place> m_places;
    vector<Transition> ml_transitions;

};

#endif // !defined(AFX_PETRINET_H__91DDDBD6_8669_4EC2_A0F5_E20F2FAFDC37__INCLUDED_)
