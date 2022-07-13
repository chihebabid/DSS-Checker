//
// Created by chiheb on 13/07/22.
//

#include "ModuleSS.h"

MetaState* ModuleSS::getInitialMS() {
    return mlMS[0];
}
/*
 * Try to insert a new MetaState
 * @return : false if it is already exists
 */
bool ModuleSS::insertMS(MetaState *ms) {
    mlMS.push_back(ms);
    return true;
}