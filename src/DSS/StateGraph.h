#ifndef STATEGRAPH_H
#define STATEGRAPH_H

#include "SCC.h"



class SCC;

class StateGraph {
public:
    StateGraph();

    virtual ~StateGraph();

    long getCountArcs();
    Marking *existState(Marking *marq);
    Marking *addMarquage(Marking *m);
    vector<Marking *> *getListMarquages();
    vector<SCC *> *getListSCCs();
    void computeSCCTarjan();
    SCC *findSCC(Marking *state);
private:
    void strongconnect(Marking *v);

    vector<Marking *> m_nodes;
    vector<string> m_transitions;
    vector<SCC *> m_scc;
    unsigned int m_index; // Used in Tarjan algorithm
    vector<Marking *> m_stack;
};

#endif // STATEGRAPH_H
