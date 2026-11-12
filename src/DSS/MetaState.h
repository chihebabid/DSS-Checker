/*
 * MetaState.h
 *
 *  Created on: 17 sept. 2015
 *      Author: LIP2
 */

#ifndef METASTATE_H_
#define METASTATE_H_

#include <memory>
#include <unordered_set>
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
    virtual ~MetaState(); // defined in the .cpp: destroying the sync arcs requires the definition of ArcSync

    // Sync arcs leaving this metastate; they are owned by the metastate
    const vector<unique_ptr<ArcSync>> &getSyncSucc() const;
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
    // Names of the metastates merged into this one
    [[nodiscard]] const vector<ProductSCC *> &getMergedNames() const { return m_merged_names; }
    // Destroy the sync arcs leading to destination
    void removeSyncArcsTo(const MetaState *destination);
    // Destroy all the sync arcs
    void clearSyncArcs();
    // Take ownership of sync_arc and become its parent
    void addSyncArc(unique_ptr<ArcSync> sync_arc);
    uint32_t getArcCount();
    bool operator==(MetaState &ms) const;
    [[nodiscard]] uint32_t getId() const;
    // Returns the marking of this metastate equal to *marking, nullptr if there is none
    Marking *existMarking(const Marking *marking) const;
    // Inserts m unless an equal marking exists: returns that marking, nullptr if m has been inserted
    Marking *insertMarking(Marking *m);
    void computeSCCTarjan();
    void setIdModule(const size_t id) { m_id_module = id;}
    [[nodiscard]] size_t getIdModule() const { return m_id_module;}
    size_t getTerminalSCCsCount() const;
    // True if the metastate contains a cycle of local transitions:
    // an SCC with several markings, or a marking with a loop on itself
    [[nodiscard]] bool hasLocalCycle() const;
private:
    size_t m_id_module;
    ProductSCC *m_name {};
    vector<ProductSCC *> m_merged_names; // Names of the metastates merged into this one (reduction)
    vector<unique_ptr<ArcSync>> m_sync_arcs;
    uint32_t m_id;
    static uint32_t m_counter;

    vector<Marking *> m_nodes; // In insertion order
    unordered_set<Marking *, Marking::PtrHash, Marking::PtrEqual> m_nodes_index; // Same markings, for fast lookup
    vector<string> m_transition_names;
    vector<SCC *> m_sccs;
    struct TarjanContext; // Working data of Tarjan algorithm
    void computeStrongConnectedComponents(Marking *v, TarjanContext &ctx);
};

#endif /* METASTATE_H_ */
