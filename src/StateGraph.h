#ifndef STATEGRAPH_H
#define STATEGRAPH_H
#include "SCC.h"

#include "InternalArc.h"

class InternalArc;
class SCC;
class StateGraph
{
    public:
        StateGraph();
        virtual ~StateGraph();
        long getCountNodes();
        long getCountArcs();
        Marking* existState(Marking *marq);
        Marking* addMarquage(Marking *m);
        //void addArc(InternalArc *arc);
        void setID(int module);
        vector<Marking*>* getListMarquages();
        //vector<InternalArc>* getListArcs();
        vector<SCC*>* getListSCCs();
        void computeSCCTarjan();
        SCC* findSCC(Marking* state);
    protected:


    private:
        void strongconnect(Marking *v);
        unsigned int m_module_id;
        vector<Marking*> m_nodes;
        //vector<InternalArc> m_arcs;
        vector<string> m_transitions;
        vector<SCC*> m_scc;
        unsigned int m_index; // Used in Tarjan algorithm
        vector<Marking*> m_stack;
};

#endif // STATEGRAPH_H
