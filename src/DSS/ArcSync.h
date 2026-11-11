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
private:
    TransitionFusionSet *m_fusion;
    ProductSCC m_source;
    MetaState *m_destination;
};


#endif /* ARCSYNC_H_ */
