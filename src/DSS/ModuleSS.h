//
// Created by chiheb on 13/07/22.
//

#ifndef DISTRIBUTEDSTATESPACE_MODULESS_H
#define DISTRIBUTEDSTATESPACE_MODULESS_H

#include <memory>
#include <unordered_map>
#include <vector>
#include "DSS/ArcSync.h"
#include "DSS/ChildOf.h"
#include "DSS/MetaState.h"

class DistributedSS;

// State space of one module; its parent is the distributed state space (set by DistributedSS::addModule)
class ModuleSS : public ChildOf<DistributedSS> {
public:
    ModuleSS(size_t id):m_id_module(id) {}

    bool insertMS(MetaState *ms);

    // First metastate (in insertion order) named productscc, or into which a metastate named productscc
    // has been merged; nullptr if there is none. Hash lookup in the name index
    MetaState *findMetaStateByProductSCC(ProductSCC &productscc);

    // Record that ms (a metastate of this module) is now also named name.
    // Called by MetaState::setProductSCCName() and MetaState::absorbNames()
    void indexName(MetaState *ms, const ProductSCC *name);

    size_t getMetaStateCount() const;

    MetaState *getMetaState(const int32_t &pos);

    vector<MetaState *> &getListMetaStates();

    // True if ms is (still) a metastate of this module
    [[nodiscard]] bool contains(const MetaState *ms) const { return ms->getParent() == this; }

    /*
     * Reduce the metastates by bisimulation: two metastates are equivalent if they have the same markings and,
     * for each sync arc of one, the other has a sync arc with the same fusion to an equivalent metastate.
     * The coarsest such equivalence is computed by partition refinement; each class is merged into its first
     * inserted metastate (it takes over the names of the others, the arcs leading to them are redirected).
     * @return the number of metastates removed
     */
    size_t reduceByBisimulation();

    // Record the initial state of the module: the name of the initial metastates and the initial marking
    void setInitialState(const ProductSCC *name, const Marking &marking);

    // Initial marking, in the metastate that represents the initial name. After a reduction, this metastate
    // may be another one than the first inserted one, and its first marking may not be the initial marking
    [[nodiscard]] Marking *getInitialMarking();

    void computeSyncClosedSCC(const MetaState *ms);

    // If ms has a cycle of local transitions, mark all the SCCs of the metastates of the other modules
    // having the same name (see SCC::othersCanLoop)
    void markOthersCanLoop(const MetaState *ms);

    size_t getIdModule() const { return m_id_module; }

private:
    /*
     * Source products of the sync arcs leaving ms that are built from *scc (component of this module)
     * and, for the other modules, exclusively from terminal SCCs or "-" (any).
     * Only the maximal products are kept: if p1 > p2 then only p1 is kept (and one of equivalent products).
     * The products belong to the arcs of ms: the pointers are valid as long as these arcs exist.
     */
    std::vector<const ProductSCC *> getMaxProductOfTerminalSCCs(const MetaState *ms, const SCC *scc) const;
    vector<MetaState *> m_metastates;
    size_t m_id_module;
    // Name index: hash of a name -> metastates having this name (entries are checked by a full comparison,
    // so an outdated entry is harmless)
    std::unordered_multimap<size_t, MetaState *> m_name_index;
    // Initial state (see setInitialState())
    const ProductSCC *m_initial_name {nullptr};
    std::unique_ptr<Marking> m_initial_marking;
    // Next insertion rank (see MetaState::getRank()): among several candidates, the first inserted one is returned
    size_t m_next_rank {};
};


#endif //DISTRIBUTEDSTATESPACE_MODULESS_H
