//
// Created by chiheb on 04/10/23.
//

#include "SpotMC/DSSIterator.h"
#include "Petri/Transition.h"
#include "SpotMC/DSSKripke.h"
DSSIterator::DSSIterator(const DSSStateKey &state, const DSSKripke *kripke)
        : m_kripke(kripke), m_succ(kripke->getSuccessors(state)),
          m_state_label(kripke->hasPredicates() ? kripke->stateLabel(state.marking) : bdd(bddtrue)) {
}


bool DSSIterator::first() {
    m_current_edge = 0;
    return m_succ.size() != 0;              // There exists a successor.
}

bool DSSIterator::next() {
    ++m_current_edge;
    return m_current_edge < m_succ.size();          // More successors?
}

bool DSSIterator::done() const {
    return m_current_edge == m_succ.size();
}

DSSState *DSSIterator::dst() const {
    return new DSSState(m_succ[m_current_edge].first);
}

bdd DSSIterator::cond() const {
    // Only build the conjunction when the formula mixes both kinds of propositions
    if (!m_kripke->hasPredicates()) return m_kripke->edgeCondition(m_succ[m_current_edge].second);
    if (!m_kripke->hasTransitionAps()) return m_state_label;
    return m_state_label & m_kripke->edgeCondition(m_succ[m_current_edge].second);
}
