// Fusion.cpp: implementation of the Fusion class.
//
//////////////////////////////////////////////////////////////////////

#include "Transition.h"
#include "TransitionFusionSet.h"

void TransitionFusionSet::fire() {
    for (const auto & transition :m_transitions)
        transition->fire();
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
    for (const auto & transition : m_transitions) {
        if (transition->getPetri() == index_module)
            if (transition->getInputPlacesCount() > 0) {
                return true;
            }
    }
    return false;
}

/////////////////////////////////////////////////////////////////////
// Indiquer si le module 'index_module' participe � la communication
///////////////////////////////////////////////////////////////////////
bool TransitionFusionSet::participate(const int index_module) {

    for (const auto & transition :  m_transitions) {
        if (transition->getPetri() == index_module) {
            return true;
        }
    }
    return false;
}


bool TransitionFusionSet::isFranchissableLocal(int module) {
    for (const auto & transition :  m_transitions) {
        if (transition->getPetri() == module) {
            return transition->isLocallyFirable();
        }
    }
    return false;
}

Transition *TransitionFusionSet::getTransitionOfModule(int module) const{
    return m_transitions[module];
}