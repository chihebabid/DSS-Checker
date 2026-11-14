/*
 * ArcSync.h
 *
 *  Created on: 29 sept. 2015
 *      Author: biba
 */

#ifndef ARCSYNC_H_
#define ARCSYNC_H_

#include "DSS/ChildOf.h"
#include "DSS/ProductSCC.h"
#include "Petri/TransitionFusionSet.h"

class ProductSCC;
class MetaState;

// A synchronisation arc; its parent is the metastate it leaves, which owns it (see MetaState::addSyncArc)
class ArcSync : public ChildOf<MetaState> {
public:
    ArcSync()=default;
    ~ArcSync()=default;

    [[nodiscard]] TransitionFusionSet *getFusion() const;

    void setDestination(MetaState *destination);

    void setData(ProductSCC source, TransitionFusionSet *transition, MetaState *destination);

    [[nodiscard]] MetaState *getDestination() const;

    // The source product; it is no longer available once the arc is compact (see compact())
    [[nodiscard]] const ProductSCC &getSourceProduct() const;
    ProductSCC &getSourceProduct() { return m_source; }

    // True if module is synchronised on the fusion of this arc.
    // If not, the component of this module in the source product is "any" (see ProductSCC::isAny):
    // it stands for any SCC of the module's metastate.
    // The source product is the single source of truth: it must be built accordingly.
    [[nodiscard]] bool isSynchronised(size_t module) const;

    // SCC of module in the source product, or nullptr if module is not synchronised on this arc
    [[nodiscard]] SCC *getSourceSCC(size_t module) const;

    // Keep only the component of module (the module of the metastate of the arc) of the source product, and free
    // the others: once the DSS is built, only the sync-closure needs them. Afterwards, isSynchronised() and
    // getSourceSCC() may only be called with this module
    void compact(size_t module) { if (!isCompact()) m_source.keepOnly(module); }
    [[nodiscard]] bool isCompact() const { return m_source.isReduced(); }
private:
    TransitionFusionSet *m_fusion;
    ProductSCC m_source;
    MetaState *m_destination;
};


#endif /* ARCSYNC_H_ */
