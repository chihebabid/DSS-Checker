// Fusion.cpp: implementation of the Fusion class.
//
//////////////////////////////////////////////////////////////////////

#include "Transition.h"
#include "TransitionFusionSet.h"

void TransitionFusionSet::fire() {
    for (int i = 0; i < m_transitions.size(); i++) m_transitions[i]->fire();
}


void TransitionFusionSet::addTransition(Transition *transition) {
    m_transitions.push_back(transition);
}

bool TransitionFusionSet::isFranchissable() {
    bool franchissable = true;
    for (int i = 0; i < m_transitions.size() && franchissable; i++)
        franchissable = m_transitions[i]->isLocallyFirable();
    return franchissable;
}

string TransitionFusionSet::getFusionName() {
    return m_transitions[0]->getName();
}

int TransitionFusionSet::getLocalTransitionsCount() {
    return m_transitions.size();
}


/////////////////////////////////////////////////////////////////////
// Indiquer si le module 'index_module' participe � la communication
///////////////////////////////////////////////////////////////////////
bool TransitionFusionSet::participatePartially(const int index_module) {
    for (int i = 0; i < m_transitions.size(); i++) {
        if (m_transitions[i]->getPetri() == index_module)
            if (m_transitions[i]->getInputPlacesCount() > 0) {
                return true;
            }
    }
    return false;
}

/////////////////////////////////////////////////////////////////////
// Indiquer si le module 'index_module' participe � la communication
///////////////////////////////////////////////////////////////////////
bool TransitionFusionSet::participate(const int index_module) {

    for (int i = 0; i < m_transitions.size(); i++) {
        if (m_transitions[i]->getPetri() == index_module) {
            return true;
        }
    }
    return false;
}


bool TransitionFusionSet::isFranchissableLocal(int module) {
    bool result = false;
    for (int i = 0; i < m_transitions.size() && !result; i++) {
        if (m_transitions[i]->getPetri() == module) {
            return m_transitions.at(i)->isLocallyFirable();
        }
    }
    return false;
}

Transition *TransitionFusionSet::getTransitionOfModule(int module) const{
    return m_transitions[module];
}