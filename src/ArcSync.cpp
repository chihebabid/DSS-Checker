/*
 * ArcSync.cpp
 *
 *  Created on: 29 sept. 2015
 *      Author: biba
 */

#include "ArcSync.h"

ArcSync::ArcSync() {
}

ArcSync::~ArcSync() {
}


MetaState *ArcSync::getMetaStateDest() {
    return m_destination;
}


void ArcSync::setData(ProductSCC source, Fusion *transition, MetaState *destination) {
    m_fusion = transition;
    m_destination = destination;
    m_source = source;
}


void ArcSync::setDestination(MetaState *destination) {
    m_destination = destination;
}

Fusion *ArcSync::getFusion() {
    return m_fusion;
}

ProductSCC *ArcSync::getStartProduct() {
    return &m_source;
}
