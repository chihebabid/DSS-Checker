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
    // Destroys its sync arcs, markings and SCCs (not its names: they are shared with other metastates).
    // Defined in the .cpp: destroying the sync arcs requires the definition of ArcSync
    ~MetaState();

    // Sync arcs leaving this metastate; they are owned by the metastate
    const vector<unique_ptr<ArcSync>> &getSyncSucc() const;
    [[nodiscard]] const vector<Marking *> &getListMarkings() const;
    vector<SCC *> *getListSCCs() const;
    SCC *findSCC(Marking *state) const;
    SCC *getInitialSCC() const;
    ProductSCC *getProductSCCName() const;
    // Also records the name in the index of the module of this metastate (if it has been inserted in one)
    void setProductSCCName(ProductSCC *name);
    // True if productscc is the name of this metastate or of a metastate merged into it
    bool representsProductSCC(const ProductSCC &productscc) const;
    // Take over the names of a metastate that has been merged into this one
    void absorbNames(const MetaState &merged);
    // Names of the metastates merged into this one
    [[nodiscard]] const vector<ProductSCC *> &getMergedNames() const { return m_merged_names; }
    // Destroy all the sync arcs
    void clearSyncArcs();
    // Metastate into which this one has been merged during the construction (on the fly reduction), nullptr if none
    void setMergedInto(MetaState *ms) { m_merged_into = ms; }
    // Metastate representing this one: this one if it has not been merged during the construction, otherwise
    // the representative of the metastate it has been merged into (the chain is compressed)
    MetaState *getRepresentative();
    // Take ownership of sync_arc and become its parent
    void addSyncArc(unique_ptr<ArcSync> sync_arc);
    // Remove the arcs equal to a previous one (same label, same destination, equivalent start products), keeping the
    // order of the others; returns the number of removed arcs
    size_t removeDuplicateSyncArcs();
    // Insertion rank of the metastate in its module (set by ModuleSS::insertMS())
    [[nodiscard]] size_t getRank() const { return m_rank; }
    void setRank(size_t rank) { m_rank = rank; }
    uint32_t getArcCount();
    // Same markings (the order does not matter); the content hashes are compared first
    bool operator==(const MetaState &ms) const;
    // Hash of the markings of the metastate (independent of their order), consistent with operator==.
    // Computed once: the markings of a metastate do not change after its computation
    [[nodiscard]] size_t contentHash() const;
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
    MetaState *m_merged_into {nullptr};  // See setMergedInto()
    vector<unique_ptr<ArcSync>> m_sync_arcs;
    uint32_t m_id;
    static uint32_t m_counter;

    vector<Marking *> m_nodes; // In insertion order
    unordered_set<Marking *, Marking::PtrHash, Marking::PtrEqual> m_nodes_index; // Same markings, for fast lookup
    mutable size_t m_content_hash {};       // Cache of contentHash()
    size_t m_rank {};
    mutable bool m_content_hash_valid {false};
    vector<SCC *> m_sccs;
};

#endif /* METASTATE_H_ */
