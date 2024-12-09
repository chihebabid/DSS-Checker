// Fusion.h: interface for the Fusion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FUSION_H__9DAA3441_B97B_42ED_8A50_7B522D84BEC6__INCLUDED_)
#define AFX_FUSION_H__9DAA3441_B97B_42ED_8A50_7B522D84BEC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Transition.h"
#include "Node.h"


class TransitionFusionSet : public Node {
public:
    TransitionFusionSet()=default;

    ~TransitionFusionSet() override=default;

    bool participatePartially(const int index_module);

    bool isFranchissableLocal(int module);

    bool participate(const int index_module);

    void fire();

    int getLocalTransitionsCount();

    string getFusionName();

    bool isFranchissable();

    void addTransition(Transition *transition);

    Transition *getTransitionOfModule(int module) const;
private:
    vector<Transition *> m_transitions;

};

#endif // !defined(AFX_FUSION_H__9DAA3441_B97B_42ED_8A50_7B522D84BEC6__INCLUDED_)
