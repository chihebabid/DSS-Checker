/*
 * MetaGraph.cpp
 *
 *  Created on: 17 sept. 2015
 *      Author: LIP2
 */
#include <iostream>
#include "MetaGraph.h"

//#define ListLocalStates vector<Marking*>*



MetaGraph::~MetaGraph() {

}

void MetaGraph::addMetaState(MetaState *ms) {
    ml_metastate.push_back(ms);
}

vector<Marking*> MetaGraph::getListLocalStates(ProductSCC *productscc) {
    vector<Marking *> list_local_states;
    for (int i = 0; i < ml_metastate.size(); i++) {
        MetaState *ms = ml_metastate[i];
        if (ms->getSCCProductName() == productscc) {
            list_local_states = ms->getListMarkings();
            break;
        }
    }
    return list_local_states;
}

MetaState *MetaGraph::findMetaStateByProductSCC(ProductSCC *productscc) {

    for (unsigned int i = 0; i < ml_metastate.size(); i++) {
        if (*ml_metastate[i]->getSCCProductName() == *productscc) return ml_metastate[i];
    }
    return nullptr;
}

int MetaGraph::getMetaStateCount() {
    return ml_metastate.size();
}

MetaState *MetaGraph::getMetaState(const int pos) {
    return ml_metastate[pos];
}
