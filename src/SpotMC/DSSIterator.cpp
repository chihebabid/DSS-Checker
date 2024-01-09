//
// Created by chiheb on 04/10/23.
//

#include "DSSIterator.h"
#include "Transition.h"

DSSIterator::DSSIterator(SCC *scc, bdd cnd) : m_scc(scc), kripke_succ_iterator(cnd) {
    auto ms{m_scc->getMetaState()};

    // Transitions inside a SCC
    for (const auto &source: *(scc->getListStates())) {
        auto lsucc = source->getListSucc();
        for (const auto &elt: lsucc) {
            Transition *t = elt.first;
            Marking *m = elt.second;
            m_lsucc.emplace_back(m->getSCCContainer(), t);
        }
    }
    // Succ sucessors
    for (auto it_succ = m_scc->beginSucc(); it_succ != m_scc->endSucc(); ++it_succ) {
        m_lsucc.emplace_back(it_succ->first, it_succ->second);
    }
}


bool DSSIterator::first() {
    m_current_edge = 0;
    return m_lsucc.size() != 0;              // There exists a successor.
}

bool DSSIterator::next() {
    m_current_edge++;
    return m_current_edge < m_lsucc.size();          // More successors?
}

bool DSSIterator::done() const {
    return m_current_edge == m_lsucc.size();
}

DSSState *DSSIterator::dst() const {
    return new DSSState(m_lsucc[m_current_edge].first);
}

bdd DSSIterator::cond() const {
    spot::formula f = spot::formula::ap(m_lsucc[m_current_edge].second->getName());
    spot::bdd_dict *p = m_dict_ptr->get();
    bdd result = bdd_ithvar((p->var_map.find(f))->second);
    return result & spot::kripke_succ_iterator::cond();
}

spot::bdd_dict_ptr *DSSIterator::m_dict_ptr;