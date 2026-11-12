/*
 * ArcSync.cpp
 *
 *  Created on: 29 sept. 2015
 *      Author: biba
 */

#include "DSS/ArcSync.h"

MetaState *ArcSync::getDestination() const {
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

TransitionFusionSet *ArcSync::getFusion() const {
    return m_fusion;
}

const ProductSCC &ArcSync::getSourceProduct() const {
    return m_source;
}

bool ArcSync::isSynchronised(const size_t module) const {
    return !m_source.isAny(module);
}

SCC *ArcSync::getSourceSCC(const size_t module) const {
    return isSynchronised(module) ? m_source.getSCC(module) : nullptr;
}
