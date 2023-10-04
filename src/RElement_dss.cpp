#include "RElement_dss.h"

RElement_dss::RElement_dss() {
    PElement_dss();
}

RElement_dss::~RElement_dss() {
    //dtor
}

MetaState *RElement_dss::getMetaState(int i) const {
    return ml_metastates.at(i);
}

void RElement_dss::addMetaState(MetaState *ms) {
    ml_metastates.push_back(ms);
}

void RElement_dss::setListMetaStates(vector<MetaState *> &l_metastates) {
    ml_metastates = l_metastates;
}