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

// A synchronisation arc; its parent is the metastate it leaves (set by MetaState::addSyncArc)
class ArcSync : public ChildOf<MetaState> {
public:
    ArcSync()=default;
    virtual ~ArcSync()=default;

    TransitionFusionSet *getFusion();

    void setDestination(MetaState *destination);

    void setData(ProductSCC source, TransitionFusionSet *transition, MetaState *destination);

    MetaState *getMetaStateDest();
    ProductSCC *getStartProduct();

    // True if module is synchronised on the fusion of this arc.
    // If not, the SCC of this module in the start product is not significant:
    // it stands for any SCC of the module's metastate.
    [[nodiscard]] bool isSynchronised(size_t module) const;

    // SCC of module in the start product, or nullptr if module is not synchronised on this arc
    [[nodiscard]] SCC *getSourceSCC(size_t module);
private:
    TransitionFusionSet *m_fusion;
    ProductSCC m_source;
    MetaState *m_destination;
};


#endif /* ARCSYNC_H_ */
