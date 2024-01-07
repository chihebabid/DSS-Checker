/*
 * MetaState.cpp
 *
 *  Created on: 17 sept. 2015
 *      Author: LIP2
 */
#include <algorithm>
#include "MetaState.h"

MetaState::MetaState() {
    m_id = m_Counter++;
}

MetaState::~MetaState() {

}





vector<Marking *> &MetaState::getListMarkings() {
    return m_nodes;
}

/*vector<InternalArc>* MetaState::getListArcs() {
    return m_graph->getListArcs();
}*/

vector<SCC *> *MetaState::getListSCCs() {
    return &ml_scc;
}

SCC *MetaState::findSCC(Marking *state) {
    for (int i = 0; i < ml_scc.size(); i++) {
        vector<Marking *> *list_states = ml_scc.at(i)->getListStates();
        if (std::find(list_states->begin(), list_states->end(), state) != list_states->end()) return ml_scc.at(i);
    }
    return nullptr;
}

SCC *MetaState::getInitialSCC() {
    return findSCC(m_nodes[0]);
}


ProductSCC *MetaState::getSCCProductName() {
    return m_name;
}

void MetaState::setSCCProductName(ProductSCC *name) {
    m_name = name;
}


void MetaState::addSyncArc(ArcSync *sync_arc) {
    mArcs.push_back(sync_arc);
}

vector<ArcSync *> &MetaState::getSyncSucc() {
    return mArcs;
}

uint32_t MetaState::getArcCount() {
    u_int32_t res = 0;
    for (auto elt: m_nodes)
        res += elt->getListSucc().size();
    return res;
}

bool MetaState::operator==(const MetaState &ms) {
    if (ms.getListMarkings().size() != getListMarkings().size()) return false;
    for (const auto &elt1: ms.getListMarkings()) {
        bool found = false;
        for (const auto &elt2: getListMarkings()) {
            if (*elt1 == *elt2) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }
    return true;
}

uint32_t MetaState::getId() const {
    return m_id;
}

uint32_t MetaState::m_Counter {0};

void MetaState::strongconnect(Marking *v) {
    v->index = m_index;
    v->lowlink = m_index;
    m_index++;
    m_stack.push_back(v);
    v->onstack = true;

    // Consider successors of v

    for (auto elt: v->getListSucc()) {
        Marking *w = elt.second;
        if (w->index == -1) {
            strongconnect(w);
            v->lowlink = min(v->lowlink, w->lowlink);
        } else if (w->onstack) v->lowlink = min(v->lowlink, w->index);

    }
    // If v is a root node, pop the stack and generate an SCC
    if (v->lowlink == v->index) {
        SCC *scc = new SCC();
        Marking *w;
        do {
            w = m_stack.back();
            m_stack.pop_back();
            w->onstack = false;
            scc->addState(w);
        } while (w != v);
        ml_scc.emplace_back(scc);
    }
}

Marking *MetaState::existMarking(Marking *marq) {
    bool result = false;
    int i = 0;
    for (i = 0; i < m_nodes.size() && !result; i++)
        result = (*m_nodes.at(i) == *marq);
    return result ? m_nodes.at(i - 1) : NULL;
}

Marking *MetaState::insertMarking(Marking *m) {
    Marking *p = existMarking(m);
    if (!p) m_nodes.push_back(m);
    return p;
}


void MetaState::computeSCCTarjan() {
    m_index = 0;

    for (int i = 0; i < m_nodes.size(); i++) {
        m_nodes.at(i)->onstack = false;
        m_nodes.at(i)->index = -1;
        m_nodes.at(i)->lowlink = 0;
    }

    for (int i = 0; i < m_nodes.size(); i++)
        if (m_nodes.at(i)->index == -1) strongconnect(m_nodes.at(i));
}