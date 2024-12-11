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
    ml_metastate.push_back(ms);
}

vector<Marking*> MetaGraph::getListLocalStates(ProductSCC *productscc) {
    vector<Marking *> list_local_states;
    for (const auto &ms : ml_metastate) {
        if (ms->getSCCProductName() == productscc) {
            list_local_states = ms->getListMarkings();
            break;
        }
    }
    return list_local_states;
}

MetaState *MetaGraph::findMetaStateByProductSCC(ProductSCC *productscc) {

    for (auto &ms : ml_metastate) {
        if (*(ms->getSCCProductName()) == *productscc) return ms;
    }
    return nullptr;
}

int MetaGraph::getMetaStateCount() {
    return ml_metastate.size();
}

MetaState *MetaGraph::getMetaState(const int pos) {
    return ml_metastate[pos];
}
