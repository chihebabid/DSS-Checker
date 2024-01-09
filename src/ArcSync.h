/*
 * ArcSync.h
 *
 *  Created on: 29 sept. 2015
 *      Author: biba
 */

#ifndef ARCSYNC_H_
#define ARCSYNC_H_

#include "ProductSCC.h"
#include "Fusion.h"

class ProductSCC;

class ArcSync {
public:
    ArcSync()=default;
    virtual ~ArcSync()=default;

    Fusion *getFusion();

    void setDestination(MetaState *destination);

    void setData(ProductSCC source, Fusion *transition, MetaState *destination);

    MetaState *getMetaStateDest();

    ProductSCC *getStartProduct();
private:
    Fusion *m_fusion;
    ProductSCC m_source;
    MetaState *m_destination;
};


#endif /* ARCSYNC_H_ */
