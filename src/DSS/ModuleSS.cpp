//
// Created by chiheb on 13/07/22.
//

#include "ModuleSS.h"

MetaState* ModuleSS::getInitialMS() {
    return mlMetaState[0];
}
/*
 * Try to insert a new MetaState
 * @return : false if it is already exists
 */
bool ModuleSS::insertMS(MetaState *ms) {
    mlMetaState.push_back(ms);
    return true;
}

MetaState *ModuleSS::findMetaStateByProductSCC(ProductSCC &productscc) {
    for (const auto & elt: mlMetaState) {
        if (*(elt->getSCCProductName()) == productscc) return elt;
    }
    return nullptr;
}