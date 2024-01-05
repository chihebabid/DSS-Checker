//
// Created by chiheb on 04/10/23.
//

#include "DSSIterator.h"

DSSIterator::DSSIterator ( SCC* scc,MetaState *ms, bdd cnd ) :m_scc ( scc ), kripke_succ_iterator ( cnd )
{
    StateGraph *graph=ms->getStateGraph();
    // Add local successors
    for (const auto &source: *(scc->getListStates())) {
        //auto sourceName = petri->getMarquageName(*source);
        auto lsucc = source->getListSucc();
        if (!lsucc->empty()) {
            for (const auto &elt: *lsucc) {
                Transition *t=elt.first;
                Marking *m=elt.second;
                auto dest_scc=ms->findSCC(m);
                m_lsucc.push_back({dest_scc,ms,t});
            }
        }
    }
}


bool DSSIterator::first()
{
    m_current_edge=0;
    return m_lsucc.size()!=0;              // There exists a successor.
}

bool DSSIterator::next()
{
    m_current_edge++;
    return m_current_edge<m_lsucc.size();          // More successors?
}

bool DSSIterator::done() const {

    return m_current_edge==m_lsucc.size();
}

DSSState* DSSIterator::dst() const
{
    return new DSSState(std::get<0>(m_lsucc[m_current_edge]),std::get<1>(m_lsucc[m_current_edge]));
}

bdd DSSIterator::cond()  const
{

    if (std::get<2>(m_lsucc[m_current_edge])==NULL ) {
        return bddtrue;
    }
    /*spot::formula f=spot::formula::ap (std::get<2>(m_lsucc[m_current_edge])->getName());
    spot::bdd_dict *p=m_dict_ptr->get();
    bdd   result=bdd_ithvar ( ( p->var_map.find ( f ) )->second );
    return result & spot::kripke_succ_iterator::cond();*/
    return bddtrue;
}

spot::bdd_dict_ptr* DSSIterator::m_dict_ptr;