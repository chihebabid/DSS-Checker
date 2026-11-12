/*
 * ArcSync.cpp
 *
 *  Created on: 29 sept. 2015
 *      Author: biba
 */

#include "DSS/ArcSync.h"

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

bool ArcSync::isSynchronised(const size_t module) const {
    return m_fusion->getTransitionOfModule(static_cast<int>(module)) != nullptr;
}

SCC *ArcSync::getSourceSCC(const size_t module) {
    return isSynchronised(module) ? m_source.getSCC(module) : nullptr;
}
