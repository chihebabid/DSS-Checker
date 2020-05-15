// PetriNet.h: interface for the PetriNet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PETRINET_H__91DDDBD6_8669_4EC2_A0F5_E20F2FAFDC37__INCLUDED_)
#define AFX_PETRINET_H__91DDDBD6_8669_4EC2_A0F5_E20F2FAFDC37__INCLUDED_
//#include "StdAfx.h"
#include "Marking.h"	// Added by ClassView
#include "Fils.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Graphe.h"
#include "StateGraph.h"
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
	vector<Fils> getListFilsEx(ListMarquage* groupe);
	vector<Fils> getListFils(ListMarquage* groupe);
	int addPlacesEntrees(string nom_transition, vector<string> liste_places_entrees,vector<int> liste_poids);
	bool aciveFusion(Marking *marq);
	void replaceCyclePhase1(PilePhase1 *pile, const int index,Marking* marq);
	void replaceCycle(PileRed* pile,const int index,Fils *fils);
	bool estLocalementRedondant(Marking *marq);
	Transition* getTransitionAdresse(const string nom_transition);
	void renommerTransitions(vectorString transitions);
	int getNumero();
	void setNumero(const int index);
	Automata* getLocalSpaceReduced(vector<Marking>& liste_marquages,GrapheSync* graphe_sync,Automata* graphe);
	string getMarquageName(Marking marquage);
	vector<Marking>* getListMarquageAccFrom(Marking, Automata *automata,ListMarquage** list);
	int addPlacesSorties(string nom_transition,vector<string> liste_places_sorties,vector<int> liste_poids);
	void tirer(Transition& t);
	void setMarquage(Marking* marquage);
	Marking getMarquage();
	Automata* getLocalSpace(Marking marquage);
	Transition getTransition(const int index);
	Place getPlace(const int index);
	Transition* getTransitionAdresse(const int code);
	void addListTransitions(vector<Transition> liste_transitions);
	void addListPlaces(vector<Place> liste_places);
	void addTransition(Transition transition);
	void addPlace(Place place);
	int getNbTransitionsFranchissables();
	int getTransitionsCount();
	int getPlacesCount();
	void printMarquage();
	PetriNet();
	virtual ~PetriNet();
	Graphe* getLocalGraph(Marking marquage);
	StateGraph *getStateGraph(Marking marquage);
	void printMetaState(MetaState *ms);
    void printMetaStateEx(MetaState *ms);


private:
	void addArcs(ListMarquage *list,ListMarquage* noeud);
	Automata* m_graphe;
	void update(vector<Fils>& liste_fils, Marking marq,Transition& transition);
	vector<Fils> getListeFils();
	vector<Fils> getListeFils(Marking marq);
	ListMarquage getListeFilsMarquages(Marking& marq);
	int m_numero;
//	vector<Marquage> m_list_marq_inserted;
	vector<Transition*> getListeTransitionsFranchissables();
	Place* getPlaceAdresse(string placename);
	vector<Place> m_places;
	vector<Transition> m_transitions;

};

#endif // !defined(AFX_PETRINET_H__91DDDBD6_8669_4EC2_A0F5_E20F2FAFDC37__INCLUDED_)
