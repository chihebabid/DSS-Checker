// Fusion.cpp: implementation of the Fusion class.
//
//////////////////////////////////////////////////////////////////////

#include "Petri/Transition.h"
#include "Petri/TransitionFusionSet.h"

void TransitionFusionSet::fire() {
    for (const auto & transition :m_transitions)
        transition->fire();
}


void TransitionFusionSet::addTransition(Transition *transition) {
    m_transitions.push_back(transition);
}

bool TransitionFusionSet::isFirable() {
    for (const auto transition : m_transitions)
        if (!transition->isLocallyFirable()) return false;
    return true;
}

string TransitionFusionSet::getFusionName() {
    return m_transitions[0]->getName();
}

int TransitionFusionSet::getLocalTransitionCount() {
    return m_transitions.size();
}


/*
 * @brief Check whether module is synchronised on the transition fusion set
 */
bool TransitionFusionSet::participatePartially(const int module) {
    for (const auto & transition : m_transitions) {
        if (transition->getPetri() == module)
            if (transition->getInputPlaceCount() > 0) {
                return true;
            }
    }
    return false;
}

/*
 * @brief returns true if index_module is synchronised on transition
 */
bool TransitionFusionSet::participate(const int module) {
    for (const auto & transition :  m_transitions) {
        if (transition->getPetri() == module) {
            return true;
        }
    }
    return false;
}


bool TransitionFusionSet::isLocallyFirable(int module) {
    for (const auto & transition :  m_transitions) {
        if (transition->getPetri() == module) {
            return transition->isLocallyFirable();
        }
    }
    return false;
}

Transition *TransitionFusionSet::getTransitionOfModule(int module) const{
    if (module>=m_transitions.size() or module<0) {
        return nullptr;
    }
    return m_transitions[module];
}