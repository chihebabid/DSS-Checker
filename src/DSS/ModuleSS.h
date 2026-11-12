//
// Created by chiheb on 13/07/22.
//

#ifndef DISTRIBUTEDSTATESPACE_MODULESS_H
#define DISTRIBUTEDSTATESPACE_MODULESS_H

#include "DSS/ArcSync.h"
#include "DSS/ChildOf.h"
#include "DSS/MetaState.h"

class DistributedSS;

// State space of one module; its parent is the distributed state space (set by DistributedSS::addModule)
class ModuleSS : public ChildOf<DistributedSS> {
public:
    ModuleSS(size_t id):m_id_module(id) {}

    MetaState *getInitialMS();

    bool insertMS(MetaState *ms);

    MetaState *findMetaStateByProductSCC(ProductSCC &productscc);

    friend ostream &operator<<(ostream &os, const ModuleSS &ss);

    size_t getMetaStateCount() const;

    MetaState *getMetaState(const int32_t &pos);

    vector<MetaState *> &getListMetaStates();

    void removeMetaState(MetaState *ms);

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
};


#endif //DISTRIBUTEDSTATESPACE_MODULESS_H
