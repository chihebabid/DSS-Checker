/*
 * MetaState.h
 *
 *  Created on: 17 sept. 2015
 *      Author: LIP2
 */

#ifndef METASTATE_H_
#define METASTATE_H_

#include <vector>

#include "ProductSCC.h"

using namespace std;

class ArcSync;
class StateGraph;
class ProductSCC;
class MetaState {
public:
    MetaState();
    vector<ArcSync *> &getSucc();
    vector<Marking *> *getListMarq();
    //vector<InternalArc>* getListArcs();
    vector<SCC *> *getListSCCs();
    SCC *findSCC(Marking *state);
    virtual ~MetaState();
    SCC *getInitialSCC();
    ProductSCC *getSCCProductName();
    void setSCCProductName(ProductSCC *name);
    void addSyncArc(ArcSync *sync_arc);
    uint32_t getArcCount();
    bool operator==(const MetaState &ms);
    uint32_t getId() const;
    Marking *existState(Marking *marq);
    Marking *addMarquage(Marking *m);
    void computeSCCTarjan();
private:
    //StateGraph *m_graph;
    ProductSCC *m_name;
    vector<ArcSync *> mArcs;
    uint32_t m_id;
    static uint32_t m_Counter;

    void strongconnect(Marking *v);
    vector<Marking *> m_nodes;
    vector<string> ml_transition_names;
    vector<SCC *> ml_scc;
    unsigned int m_index; // Used in Tarjan algorithm
    vector<Marking *> m_stack;
};

#endif /* METASTATE_H_ */
