//
// Created by chiheb on 04/10/23.
//

#include "SpotMC/DSSIterator.h"
#include "Petri/Transition.h"

DSSIterator::DSSIterator(SCC *scc, bdd cnd) : m_scc(scc), kripke_succ_iterator(cnd) {


    // Transitions inside an SCC
    for (const auto &source: *(scc->getListStates())) {
        const auto &lsucc = source->getListSucc();
        for (const auto &[t,m]: lsucc) {
            m_succ.emplace_back(m->getSCCContainer(), t);
        }
    }
    // Succ sucessors
    for (auto it_succ = m_scc->beginSucc(); it_succ != m_scc->endSucc(); ++it_succ) {
        m_succ.emplace_back(it_succ->first, it_succ->second);
    }

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
    spot::formula f = spot::formula::ap(m_succ[m_current_edge].second->getName());
    spot::bdd_dict *p = m_dict_ptr->get();
    if (p->var_map.find(f)==p->var_map.end()) return bdd_false();
    bdd result = bdd_ithvar((p->var_map.find(f))->second);
    return result & spot::kripke_succ_iterator::cond();
}

spot::bdd_dict_ptr *DSSIterator::m_dict_ptr;