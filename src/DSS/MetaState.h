/*
 * MetaState.h
 *
 *  Created on: 17 sept. 2015
 *      Author: LIP2
 */

#ifndef METASTATE_H_
#define METASTATE_H_

#include <vector>
#include "DSS/ChildOf.h"
#include "DSS/SCC.h"
#include "DSS/ProductSCC.h"

using namespace std;

class ArcSync;
class ProductSCC;
class ModuleSS;

// A metastate; its parent is the module state space containing it (set by ModuleSS::insertMS)
class MetaState : public ChildOf<ModuleSS> {
public:
    MetaState();
    virtual ~MetaState()=default;

    const vector<ArcSync *> &getSyncSucc() const;
    vector<Marking *> getListMarkings() const;
    vector<SCC *> *getListSCCs() const;
    SCC *findSCC(Marking *state);
    SCC *getInitialSCC();
    ProductSCC *getProductSCCName() const;
    void setProductSCCName(ProductSCC *name);
    // True if productscc is the name of this metastate or of a metastate merged into it
    bool representsProductSCC(const ProductSCC &productscc) const;
    // Take over the names of a metastate that has been merged into this one
    void absorbNames(const MetaState &merged);
    void removeSyncArc(ArcSync *sync_arc);
    void clearSyncArcs();
    void addSyncArc(ArcSync *sync_arc);
    uint32_t getArcCount();
    bool operator==(MetaState &ms) const;
    [[nodiscard]] uint32_t getId() const;
    Marking *existMarking(Marking *marking);
    Marking *insertMarking(Marking *m);
    void computeSCCTarjan();
    void setIdModule(const size_t id) { m_id_module = id;}
    [[nodiscard]] size_t getIdModule() const { return m_id_module;}
    size_t getTerminalSCCsCount() const;
private:
    size_t m_id_module;
    ProductSCC *m_name {};
    vector<ProductSCC *> m_merged_names; // Names of the metastates merged into this one (reduction)
    vector<ArcSync *> m_sync_arcs;
    uint32_t m_id;
    static uint32_t m_counter;

    vector<Marking *> m_nodes;
    vector<string> m_transition_names;
    vector<SCC *> m_sccs;
    unsigned int m_index; // Used in Tarjan algorithm
    vector<Marking *> m_stack;
    void computeStrongConnectedComponents(Marking *v);
};

#endif /* METASTATE_H_ */
