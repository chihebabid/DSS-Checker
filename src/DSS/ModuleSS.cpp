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

size_t ModuleSS::getMetaStateCount() const {
    return mlMetaState.size();
}

MetaState* ModuleSS::getMetaState(const int32_t & pos) {
    return mlMetaState[pos];
}
/*
 * @brief Return Metastates list
 */
vector<MetaState*> &ModuleSS::getLMetaState() {
    return mlMetaState;
}

/*
 * @brief Remove a metastate
 */
void ModuleSS::removeMetaState(MetaState *ms) {
    for (auto &edge: ms->getSucc()) {
        delete edge;
    }
    std::remove(mlMetaState.begin(), mlMetaState.end(),ms);
    auto compare = [&ms](ArcSync *arc) {
        return ms == arc->getMetaStateDest();
    };
    for(const auto & elt : mlMetaState) {
        auto ptr=std::find_if(elt->getSucc().begin(),elt->getSucc().end(),compare);
        if (ptr!=elt->getSucc().end()) {
            delete (*ptr);
            elt->getSucc().erase(ptr);
        }
    }
}

ostream& operator<<(ostream& os, const ModuleSS& ss)
{
    //os << dt.mo << '/' << dt.da << '/' << dt.yr;
    for (const auto & elt : ss.mlMetaState) {

    }
    return os;
}

