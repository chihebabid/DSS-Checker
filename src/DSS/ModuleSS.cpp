//
// Created by chiheb on 13/07/22.
//
#include <algorithm>
#include <iostream>
#include "DSS/ModuleSS.h"

#include "DistributedSS.h"
#include "DSS/MetaState.h"
#include "Petri/ModularPetriNet.h"

// Set to false to remove the trace of computeSyncClosedSCC()
static constexpr bool debug_sync_closed {true};

// Name of an SCC of a module (debug)
static string sccName(SCC *scc, const size_t module) {
    return ModularPetriNet::getInstance().getModule(static_cast<int>(module))->getSCCName(scc);
}

// Name of a product: the names of its SCCs, "-" for an "any" component (debug)
static string productName(const ProductSCC &product) {
    string res;
    for (size_t module {}; module < ModularPetriNet::getModuleCount(); ++module) {
        res += product.isAny(module) ? "-" : sccName(product.getSCC(module), module);
    }
    return res;
}

MetaState *ModuleSS::getInitialMS() {
    return m_metastates[0];
}

/*
 * Try to insert a new MetaState
 * @return : false if it is already exists
 */
bool ModuleSS::insertMS(MetaState *ms) {
    ms->setIdModule(m_id_module);
    ms->setParent(this);
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
    // ms itself stays alive: its SCCs are still referenced by product SCCs of other modules.
    // Its edges are owned by it: they are destroyed here.
    ms->clearSyncArcs();

    auto ptrMl = std::remove(m_metastates.begin(), m_metastates.end(), ms);
    m_metastates.erase(ptrMl, m_metastates.end());
    if (ms->getParent() == this) ms->setParent(nullptr); // detached from this module

    // Remove every remaining edge leading to ms
    for (const auto elt: m_metastates) {
        elt->removeSyncArcsTo(ms);
    }
}

/*
 * @brief Compute the sync-closure property of the terminal SCCs of all metastates
 */
void ModuleSS::computeSyncClosedSCC(const MetaState *ms) {
    const size_t module_count {ModularPetriNet::getModuleCount()};

    if (debug_sync_closed) {
        std::cout << "\n[sync-closed] ===== module " << m_id_module << ", metastate " << productName(*ms->getProductSCCName())
                  << " (id " << ms->getId() << "): " << ms->getListSCCs()->size() << " SCC(s), "
                  << ms->getTerminalSCCsCount() << " terminal, " << ms->getSyncSucc().size() << " sync arc(s)\n";
        for (const auto &sync_arc: ms->getSyncSucc()) {
            std::cout << "[sync-closed]   arc (" << productName(sync_arc->getSourceProduct()) << ", "
                      << sync_arc->getFusion()->getName() << ") -> "
                      << productName(*sync_arc->getDestination()->getProductSCCName()) << "\n";
        }
    }

    // Compute total products that should appear in sync edges
    size_t total_products{1};
    for (size_t module{}; module < module_count; ++module) {
        if (module != m_id_module) {
            auto mss {getParent()->getModule(module)};
            auto found_ms {mss->findMetaStateByProductSCC(*(ms->getProductSCCName()))};
            const size_t terminal_sccs {found_ms->getTerminalSCCsCount()};
            total_products *= terminal_sccs;
            if (debug_sync_closed) {
                std::cout << "[sync-closed]   module " << module << ": metastate " << productName(*found_ms->getProductSCCName())
                          << " (id " << found_ms->getId() << ") has " << terminal_sccs << " terminal SCC(s):";
                for (const auto &other_scc: *found_ms->getListSCCs()) {
                    if (other_scc->isTerminal()) std::cout << " " << sccName(other_scc, module);
                }
                std::cout << "\n";
            }
        }
    }
    if (debug_sync_closed) std::cout << "[sync-closed]   total_products = " << total_products << "\n";

    for (const auto &scc : *(ms->getListSCCs())) {
        if (!scc->isTerminal()) {
            if (debug_sync_closed) std::cout << "[sync-closed]   SCC " << sccName(scc, m_id_module) << ": not terminal, skipped\n";
            continue;
        }

        size_t result{};
        auto max_products {getMaxProductOfTerminalSCCs(ms, scc)};
        if (debug_sync_closed) {
            std::cout << "[sync-closed]   SCC " << sccName(scc, m_id_module) << ": terminal, "
                      << max_products.size() << " maximal product(s)\n";
        }
        for (const auto &elt : max_products) {
            const size_t represented_products {elt->getRepresentedProductsCount(m_id_module)};
            result += represented_products;
            if (debug_sync_closed) {
                std::cout << "[sync-closed]     product " << productName(*elt) << " represents "
                          << represented_products << " product(s)\n";
            }
        }

        if (debug_sync_closed) {
            std::cout << "[sync-closed]     result = " << result << ", total_products = " << total_products
                      << " => " << (total_products != result ? "sync-closed" : "NOT sync-closed") << "\n";
        }
        if (total_products != result) {
            scc->setSyncClosed(true);
        }
    }

    // for (const auto &scc : *(ms->getListSCCs())) {
    //     // We compute sync-closure only for terminal SCCs
    //     if (scc->isTerminal()) {
    //         size_t count_represented_products{};
    //         for (const auto &sync_arc : ms->getSyncSucc()) {
    //             const auto &product {sync_arc->getSourceProduct()}; // no copy
    //             if (product.getSCC(m_id_module) == scc) {
    //                 count_represented_products+=product.getRepresentedProductsCount(m_id_module);
    //             }
    //         }
    //
    //     }
    // }
}
void ModuleSS::markDivergentSyncArcs(const MetaState *ms) {
    bool others_can_loop {false};
    for (size_t module {}; module < ModularPetriNet::getModuleCount() && !others_can_loop; ++module) {
        if (module == m_id_module) continue;
        const auto found_ms {getParent()->getModule(module)->findMetaStateByProductSCC(*ms->getProductSCCName())};
        others_can_loop = found_ms && found_ms->hasLocalCycle();
    }
    for (const auto &sync_arc: ms->getSyncSucc()) {
        sync_arc->setDivergent(others_can_loop);
    }
}

std::vector<const ProductSCC *> ModuleSS::getMaxProductOfTerminalSCCs(const MetaState *ms, const SCC *scc) const {
    std::vector<const ProductSCC *> maximal_products;
    const size_t module_count {ModularPetriNet::getModuleCount()};
    for (const auto &sync_arc: ms->getSyncSucc()) {
        const auto &product {sync_arc->getSourceProduct()};
        // The component of this module must be *scc itself
        if (!product.isAny(m_id_module) and product.getSCC(m_id_module) != scc) continue;
        // The other components must be terminal SCCs or "-"
        bool only_terminal {true};
        for (size_t module {}; module < module_count && only_terminal; ++module) {
            if (module != m_id_module) only_terminal = product.isAny(module) || product.getSCC(module)->isTerminal();
        }
        if (!only_terminal) continue;
        // Keep only maximal products: skip product if a kept one is greater or equivalent ...
        const bool covered {std::any_of(maximal_products.begin(), maximal_products.end(), [&product](const ProductSCC *kept) {
            return *kept >= product;
        })};
        if (covered) continue;
        // ... and drop the kept ones that are less than product
        std::erase_if(maximal_products, [&product](const ProductSCC *kept) { return *kept < product; });
        maximal_products.push_back(&product);
    }
    return maximal_products;
}

ostream &operator<<(ostream &os, const ModuleSS &ss) {
    //os << dt.mo << '/' << dt.da << '/' << dt.yr;

    return os;
}
