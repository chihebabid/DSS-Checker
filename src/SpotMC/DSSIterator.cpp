//
// Created by chiheb on 04/10/23.
//

#include "DSSIterator.h"
#include "Transition.h"
DSSIterator::DSSIterator(SCC *scc, bdd cnd) : m_scc(scc), kripke_succ_iterator(cnd) {
    auto ms{m_scc->getMetaState()};

    // Add local successors
    for (const auto &source: *(scc->getListStates())) {
        //auto sourceName = petri->getMarquageName(*source);
        auto lsucc = source->getListSucc();

        for (const auto &elt: lsucc) {
            Transition *t = elt.first;
            Marking *m = elt.second;
            m_lsucc.emplace_back(m->getSCCContainer(), t);
        }
        // TODO : add sync closed
        // TODO : add sync edges if exists
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
    /*spot::formula f=spot::formula::ap (std::get<2>(m_lsucc[m_current_edge])->getName());
    spot::bdd_dict *p=m_dict_ptr->get();
    bdd   result=bdd_ithvar ( ( p->var_map.find ( f ) )->second );
    return result & spot::kripke_succ_iterator::cond();*/
    auto trans {m_lsucc[m_current_edge].second};

    if (trans==nullptr) return bddtrue;
    auto trans_name {trans->getName()};
    spot::bdd_dict *p=m_dict_ptr->get();
    spot::formula f=spot::formula::ap(trans_name);
    bdd   result=bdd_ithvar((p->var_map.find(f))->second);

    return result & spot::kripke_succ_iterator::cond();
}

spot::bdd_dict_ptr *DSSIterator::m_dict_ptr;