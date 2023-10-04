#include "StateGraph.h"

StateGraph::StateGraph() {

    m_nodes.clear();

}

StateGraph::~StateGraph() {
    for (int i = 0; i < m_nodes.size(); i++)
        delete m_nodes.at(i);
}

// Return nodes count
long StateGraph::getCountNodes() {
    return m_nodes.size();
}

// Return arcs count
long StateGraph::getCountArcs() {
    u_int32_t res = 0;
    for (auto elt: m_nodes)
        res += elt->getListSucc()->size();
    return res;
}

// Return true if state already exists
Marking *StateGraph::existState(Marking *marq) {
    bool result = false;
    int i = 0;
    for (i = 0; i < m_nodes.size() && !result; i++)
        result = (*m_nodes.at(i) == *marq);
    return result ? m_nodes.at(i - 1) : NULL;
}

Marking *StateGraph::addMarquage(Marking *m) {
    Marking *p = existState(m);
    if (!p) m_nodes.push_back(m);
    return p;
}

/*void StateGraph::addArc(InternalArc *arc)
{
    mArcs.push_back(*arc);
}*/

void StateGraph::setID(int module) {
    m_module_id = module;
}


vector<Marking *> *StateGraph::getListMarquages() {
    return &m_nodes;
}

/*vector<InternalArc>* StateGraph::getListArcs()
{
    return &mArcs;
}*/

vector<SCC *> *StateGraph::getListSCCs() {
    return &m_scc;
}


void StateGraph::computeSCCTarjan() {
    m_index = 0;

    for (int i = 0; i < m_nodes.size(); i++) {
        m_nodes.at(i)->onstack = false;
        m_nodes.at(i)->index = -1;
        m_nodes.at(i)->lowlink = 0;
    }

    for (int i = 0; i < m_nodes.size(); i++)
        if (m_nodes.at(i)->index == -1) strongconnect(m_nodes.at(i));
}

void StateGraph::strongconnect(Marking *v) {
    v->index = m_index;
    v->lowlink = m_index;
    m_index++;
    m_stack.push_back(v);
    v->onstack = true;

    // Consider successors of v

    for (auto elt: *(v->getListSucc())) {
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
        m_scc.push_back(scc);
    }
}

SCC *StateGraph::findSCC(Marking *state) {

    for (int i = 0; i < m_scc.size(); i++) {
        vector<Marking *> *list_states = m_scc.at(i)->getListStates();
        if (find(list_states->begin(), list_states->end(), state) != list_states->end()) return m_scc.at(i);
    }
    return NULL;
}
