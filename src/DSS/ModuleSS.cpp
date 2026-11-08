//
// Created by chiheb on 13/07/22.
//
#include <algorithm>
#include "ModuleSS.h"
#include "MetaState.h"

MetaState *ModuleSS::getInitialMS() {
    return m_metastates[0];
}

/*
 * Try to insert a new MetaState
 * @return : false if it is already exists
 */
bool ModuleSS::insertMS(MetaState *ms) {
    ms->setIdModule(m_id_module);
    m_metastates.push_back(ms);
    return true;
}

MetaState *ModuleSS::findMetaStateByProductSCC(ProductSCC &productscc) {
    for (const auto &elt: m_metastates) {
        if (*(elt->getProductSCCName()) == productscc) return elt;
    }
    return nullptr;
}

size_t ModuleSS::getMetaStateCount() const {
    return m_metastates.size();
}

MetaState *ModuleSS::getMetaState(const int32_t &pos) {
    return m_metastates[pos];
}

/*
 * @brief Return Metastates list
 */
vector<MetaState *> &ModuleSS::getListMetaStates() {
    return m_metastates;
}

/*
 * @brief Remove a metastate
 */
void ModuleSS::removeMetaState(MetaState *ms) {
    for (auto edge: ms->getSyncSucc()) {
        delete edge;
    }

    auto ptrMl = std::remove(m_metastates.begin(), m_metastates.end(), ms);
    m_metastates.erase(ptrMl, m_metastates.end());

    auto compare = [&ms](ArcSync *arc) -> bool {
        return ms == arc->getMetaStateDest();
    };

    for (const auto elt: m_metastates) {
        if (!elt->getSyncSucc().empty()) {
            auto ptr = std::find_if(begin(elt->getSyncSucc()), end(elt->getSyncSucc()), compare);
            if (ptr != elt->getSyncSucc().end()) {
                //cout<<"erassed\n";
                //cout<<"size before : "<<elt->getSyncSucc().size()<<endl;
                elt->getSyncSucc().erase(ptr);
                //cout<<"size after : "<<elt->getSyncSucc().size()<<endl;
                delete (*ptr);
            }
        }
    }
}

ostream &operator<<(ostream &os, const ModuleSS &ss) {
    //os << dt.mo << '/' << dt.da << '/' << dt.yr;

    return os;
}

