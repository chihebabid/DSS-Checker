//
// Created by chiheb on 13/07/22.
//
#include <algorithm>
#include "DSS/ModuleSS.h"

#include "DistributedSS.h"
#include "DSS/MetaState.h"
#include "Petri/ModularPetriNet.h"

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
        if (elt->representsProductSCC(productscc)) return elt;
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
    // ms itself stays alive: its SCCs are still referenced by product SCCs of other modules.
    // Its edges are gone, so do not keep dangling pointers on them.
    ms->clearSyncArcs();

    auto ptrMl = std::remove(m_metastates.begin(), m_metastates.end(), ms);
    m_metastates.erase(ptrMl, m_metastates.end());

    auto compare = [&ms](ArcSync *arc) -> bool {
        return ms == arc->getMetaStateDest();
    };

    // Remove every remaining edge leading to ms
    for (const auto elt: m_metastates) {
        const auto edges {elt->getSyncSucc()}; // copy: the list is modified below
        for (auto edge: edges) {
            if (compare(edge)) {
                elt->removeSyncArc(edge);
                delete edge;
            }
        }
    }
}

/*
 * @brief Compute the sync-closure property of the terminal SCCs of all metastates
 */
void ModuleSS::computeSyncClosedSCC(const MetaState *ms) {
    // Compute total products that should appear in sync edges
    size_t total_products{1};
    for (size_t module{};module<ModularPetriNet::getModuleCount();++module) {
        if (module!=m_id_module) {
            auto mss {m_parent->getModule(module)};
            auto found_ms {mss->findMetaStateByProductSCC(*(ms->getProductSCCName()))};
            total_products *= found_ms->getTerminalSCCsCount();
        }
    }
    for (const auto &scc : *(ms->getListSCCs())) {
        // We compute sync-closure only for terminal SCCs
        if (scc->isTerminal()) {
            size_t count_represented_products{};
            for (const auto &sync_arc : ms->getSyncSucc()) {
                auto product {sync_arc->getStartProduct()};
                if (product->getSCC(m_id_module) == scc) {
                    count_represented_products+=product->getRepresentedProductsCount(*(sync_arc->getFusion()),m_id_module);
                }
            }
            if (total_products==count_represented_products) {
                scc->setSyncClosed(true);
            }
        }
    }
}

ostream &operator<<(ostream &os, const ModuleSS &ss) {
    //os << dt.mo << '/' << dt.da << '/' << dt.yr;

    return os;
}

