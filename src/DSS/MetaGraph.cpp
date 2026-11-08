/*
 * MetaGraph.cpp
 *
 *  Created on: 17 sept. 2015
 *      Author: LIP2
 */

#include "MetaGraph.h"

MetaGraph::~MetaGraph() {

}

void MetaGraph::addMetaState(MetaState *ms) {
    m_metastates.push_back(ms);
}

vector<Marking*> MetaGraph::getListLocalStates(ProductSCC *productscc) {
    vector<Marking *> list_local_states;
    for (const auto &ms : m_metastates) {
        if (ms->getProductSCCName() == productscc) {
            list_local_states = ms->getListMarkings();
            break;
        }
    }
    return list_local_states;
}

MetaState *MetaGraph::findMetaStateByProductSCC(ProductSCC *productscc) {

    for (auto &ms : m_metastates) {
        if (*(ms->getProductSCCName()) == *productscc) return ms;
    }
    return nullptr;
}

int MetaGraph::getMetaStateCount() {
    return m_metastates.size();
}

MetaState *MetaGraph::getMetaState(const int pos) {
    return m_metastates[pos];
}
