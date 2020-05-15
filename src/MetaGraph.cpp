/*
 * MetaGraph.cpp
 *
 *  Created on: 17 sept. 2015
 *      Author: LIP2
 */
#include <iostream>
#include "MetaGraph.h"
#define ListLocalStates vector<Marking*>*
MetaGraph::MetaGraph() {
	ml_metastate.clear();

}

MetaGraph::~MetaGraph() {
	// TODO Auto-generated destructor stub
}

void MetaGraph::addMetaState(MetaState *ms){

	ml_metastate.push_back(ms);
}

ListLocalStates MetaGraph::getListLocalStates(ProductSCC* productscc) {
    bool result=false;
    vector<Marking*>* list_local_states=NULL;
    for (int i=0;i<ml_metastate.size() && !result;i++) {
        MetaState* ms=ml_metastate.at(i);
        if (ms->getSCCProductName()==productscc) {
            list_local_states=ms->getListMarq();
            result=true;
        }
    }
    return list_local_states;
}
MetaState* MetaGraph::findMetaStateByProductSCC(ProductSCC* productscc) {

    for (unsigned int i=0;i<ml_metastate.size() ;i++) {
        if (*ml_metastate.at(i)->getSCCProductName()==*productscc) return ml_metastate.at(i);
    }
    return NULL;
}

int MetaGraph::getMetaStateCount() {
    return ml_metastate.size();
}

MetaState * MetaGraph::getMetaState(const int pos) {
    return ml_metastate.at(pos);
}
