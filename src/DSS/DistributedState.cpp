/*
 * DistributedState.cpp
 *
 *  Created on: 17 sept. 2015
 *      Author: LIP2
 */

#include "DSS/DistributedState.h"

DistributedState::DistributedState(ModularPetriNet *petri):m_petri(petri) {

}






MetaGraph *DistributedState::getMetaGraph(const int index) {
    return m_metagraphs[index];
}

void DistributedState::setModuleCount(const int n) {
    m_metagraphs.resize(n);
    for (int i = 0; i < n; i++)
        m_metagraphs[i] = new MetaGraph();

}

void DistributedState::addMetaState(MetaState *ms, int module) {
    m_metagraphs[module]->addMetaState(ms);
}

vector<Marking *> DistributedState::getLocalStates(ProductSCC *productscc, const int module) {
    return m_metagraphs[module]->getListLocalStates(productscc);
}
