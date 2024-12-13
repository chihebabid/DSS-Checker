/*
 * ArcSync.cpp
 *
 *  Created on: 29 sept. 2015
 *      Author: biba
 */

#include "ArcSync.h"

MetaState *ArcSync::getMetaStateDest() {
    return m_destination;
}


void ArcSync::setData(ProductSCC source, TransitionFusionSet *transition, MetaState *destination) {
    m_fusion = transition;
    m_destination = destination;
    m_source = source;
}


void ArcSync::setDestination(MetaState *destination) {
    m_destination = destination;
}

TransitionFusionSet *ArcSync::getFusion() {
    return m_fusion;
}

ProductSCC *ArcSync::getStartProduct() {
    return &m_source;
}
