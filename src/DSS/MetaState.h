/*
 * MetaState.h
 *
 *  Created on: 17 sept. 2015
 *      Author: LIP2
 */

#ifndef METASTATE_H_
#define METASTATE_H_

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "DSS/ChildOf.h"
#include "DSS/LocalGraph.h"
#include "DSS/SCC.h"
#include "DSS/ProductSCC.h"

using namespace std;

class ArcSync;
class ProductSCC;
class ModuleSS;

/*
 * A metastate; its parent is the module state space containing it (set by ModuleSS::insertMS). It has:
 *  - its name (and the names of the metastates merged into it);
 *  - its markings and their SCCs: a local graph, shared by all the metastates having the same markings;
 *  - the sync-closure of its SCCs (and whether another module can loop forever): they depend on its sync arcs and
 *    on its name, not only on its markings;
 *  - its successors by sync arcs.
 */
class MetaState : public ChildOf<ModuleSS> {
public:
    // graph: its markings and their SCCs (not owned)
    explicit MetaState(LocalGraph *graph = nullptr);
    // Destroys its sync arcs (not its local graph, which may be shared, nor its names, shared with other metastates).
    // Defined in the .cpp: destroying the sync arcs requires the definition of ArcSync
    ~MetaState();

    // ---- Name
    ProductSCC *getProductSCCName() const;
    // Also records the name in the index of the module of this metastate (if it has been inserted in one)
    void setProductSCCName(ProductSCC *name);
    // True if productscc is the name of this metastate or of a metastate merged into it
    bool representsProductSCC(const ProductSCC &productscc) const;
    // Take over the names of a metastate that has been merged into this one
    void absorbNames(const MetaState &merged);
    // Names of the metastates merged into this one
    [[nodiscard]] const vector<ProductSCC *> &getMergedNames() const { return m_merged_names; }

    // ---- Markings and SCCs (local graph)
    [[nodiscard]] LocalGraph *getLocalGraph() const { return m_graph; }
    [[nodiscard]] const vector<Marking *> &getListMarkings() const { return m_graph->getListMarkings(); }
    [[nodiscard]] const vector<SCC *> *getListSCCs() const { return &m_graph->getListSCCs(); }
    // SCC of the marking of this metastate equal to *state, nullptr if there is none
    SCC *findSCC(Marking *state) const;
    [[nodiscard]] SCC *getInitialSCC() const { return m_graph->getInitialSCC(); }
    // Returns the marking of this metastate equal to *marking, nullptr if there is none
    [[nodiscard]] Marking *existMarking(const Marking *marking) const { return m_graph->existMarking(marking); }
    [[nodiscard]] size_t getTerminalSCCsCount() const { return m_graph->getTerminalSCCsCount(); }
    // True if the metastate contains a cycle of local transitions
    [[nodiscard]] bool hasLocalCycle() const { return m_graph->hasLocalCycle(); }
    uint32_t getArcCount() const { return m_graph->getArcCount(); }
    // Same markings (equal local graphs; they are shared, so that this is usually the same graph)
    bool operator==(const MetaState &ms) const { return *m_graph == *ms.m_graph; }
    // Hash of the markings, consistent with operator==
    [[nodiscard]] size_t contentHash() const { return m_graph->contentHash(); }

    // ---- Sync-closure of the SCCs (of the local graph) in this metastate
    [[nodiscard]] bool isSyncClosed(const SCC *scc) const { return hasFlag(scc, sync_closed_flag); }
    void setSyncClosed(const SCC *scc) { m_scc_flags[scc] |= sync_closed_flag; }
    // Another module, in its metastate of the same name, has a cycle of local transitions: it may loop forever, so
    // the module may stay forever in scc (executions without fairness)
    [[nodiscard]] bool othersCanLoop(const SCC *scc) const { return hasFlag(scc, others_can_loop_flag); }
    void setOthersCanLoop(const SCC *scc) { m_scc_flags[scc] |= others_can_loop_flag; }

    // ---- Successors by sync arcs
    // Sync arcs leaving this metastate; they are owned by the metastate
    const vector<unique_ptr<ArcSync>> &getSyncSucc() const;
    // Take ownership of sync_arc and become its parent
    void addSyncArc(unique_ptr<ArcSync> sync_arc);
    // Destroy all the sync arcs
    void clearSyncArcs();
    // Compact the sync arcs (see ArcSync::compact()), then remove the arcs that have become equal (same label,
    // same destination, equal component of the module); returns the number of removed arcs
    size_t compactSyncArcs();

    // ---- Construction and reduction
    // Metastate into which this one has been merged during the construction (on the fly reduction), nullptr if none
    void setMergedInto(MetaState *ms) { m_merged_into = ms; }
    // Metastate representing this one: this one if it has not been merged during the construction, otherwise
    // the representative of the metastate it has been merged into (the chain is compressed)
    MetaState *getRepresentative();
    // Insertion rank of the metastate in its module (set by ModuleSS::insertMS())
    [[nodiscard]] size_t getRank() const { return m_rank; }
    void setRank(size_t rank) { m_rank = rank; }
    [[nodiscard]] uint32_t getId() const;
    void setIdModule(const size_t id) { m_id_module = id;}
    [[nodiscard]] size_t getIdModule() const { return m_id_module;}
private:
    static constexpr uint8_t sync_closed_flag {1}, others_can_loop_flag {2};
    [[nodiscard]] bool hasFlag(const SCC *scc, const uint8_t flag) const {
        const auto it {m_scc_flags.find(scc)};
        return it != m_scc_flags.end() && (it->second & flag);
    }

    ProductSCC *m_name {};
    vector<ProductSCC *> m_merged_names; // Names of the metastates merged into this one (reduction)
    LocalGraph *m_graph;                 // Markings and SCCs, shared (not owned)
    unordered_map<const SCC *, uint8_t> m_scc_flags; // Flags of the SCCs of m_graph in this metastate (absent: none)
    vector<unique_ptr<ArcSync>> m_sync_arcs;
    MetaState *m_merged_into {nullptr};  // See setMergedInto()
    size_t m_id_module {};
    size_t m_rank {};
    uint32_t m_id;
    static uint32_t m_counter;
};

#endif /* METASTATE_H_ */
