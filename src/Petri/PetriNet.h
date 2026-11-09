// PetriNet.h: interface for the PetriNet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PETRINET_H__91DDDBD6_8669_4EC2_A0F5_E20F2FAFDC37__INCLUDED_)
#define AFX_PETRINET_H__91DDDBD6_8669_4EC2_A0F5_E20F2FAFDC37__INCLUDED_

#include <set>
#include "DSS/Marking.h"
#include "Petri/Place.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Transition;

class ListMarking;

class MetaState;

class SCC;

using std::vector;
typedef vector<string> vectorString;

class PetriNet {
public:
    string getSCCName(SCC *scc);

    int addInputPlaces(string transition_name, vector<string> input_places, vector<int> weights);

    Transition *getTransitionAddress(const string transition_name);

    int getNumber();

    void setNumber(const int index);

    string getMarkingName(Marking marking);

    int addOutputPlaces(string transition_name, vector<string> output_places, vector<int> weights);

    void fire(Transition &t);

    void setMarking(Marking *marking);

    Marking getMarking();

    Place getPlace(const int index);

    Transition *getTransitionAddress(const int code);

    void addListTransitions(vector<Transition> transitions);

    void addListPlaces(vector<Place> places);

    int getPlaceCount();

    PetriNet()=default;

    virtual ~PetriNet()=default;

    MetaState *getMetaState(Marking marking);

    void printMetaStateEx(MetaState *ms);

    bool areTransitionsIncluded(const std::set<string> &list_transitions);
private:
    int m_number;

    vector<Transition *> getListFirableTransitions();

    Place *getPlaceAddress(string placename);

    vector<Place> m_places;
    vector<Transition> m_transitions;

};

#endif // !defined(AFX_PETRINET_H__91DDDBD6_8669_4EC2_A0F5_E20F2FAFDC37__INCLUDED_)
