//
// Created by chiheb on 13/07/22.
//
#include <algorithm>
#include <set>
#include <vector>
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
    std::vector<std::vector<SCC *>> terminal_sccs_of(module_count); // terminal SCCs of the other modules
    for (size_t module{}; module < module_count; ++module) {
        if (module != m_id_module) {
            auto mss {getParent()->getModule(module)};
            auto found_ms {mss->findMetaStateByProductSCC(*(ms->getProductSCCName()))};
            for (const auto &other_scc: *found_ms->getListSCCs()) {
                if (other_scc->isTerminal()) terminal_sccs_of[module].push_back(other_scc);
            }
            const size_t terminal_sccs {terminal_sccs_of[module].size()};
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

        // Products of terminal SCCs of the other modules covered by at least one sync arc.
        // A product is counted once even if several arcs (e.g. of different fusions) cover it.
        // Each product is a tuple of indices in terminal_sccs_of ("-" stands for every terminal SCC)
        std::set<std::vector<size_t>> covered;
        auto max_products {getMaxProductOfTerminalSCCs(ms, scc)};
        if (debug_sync_closed) {
            std::cout << "[sync-closed]   SCC " << sccName(scc, m_id_module) << ": terminal, "
                      << max_products.size() << " maximal product(s)\n";
        }
        for (const auto &elt : max_products) {
            // Possible terminal SCCs (indices) of each other module for this product
            std::vector<std::vector<size_t>> choices;
            for (size_t module{}; module < module_count; ++module) {
                if (module == m_id_module) continue;
                const auto &terminals {terminal_sccs_of[module]};
                std::vector<size_t> indices;
                for (size_t i{}; i < terminals.size(); ++i) {
                    // SCCs are compared by content: equal SCCs may belong to distinct (merged) metastates
                    if (elt->isAny(module) || terminals[i]->isEqual(*elt->getSCC(module))) indices.push_back(i);
                }
                choices.push_back(std::move(indices));
            }
            // Cartesian product of the choices
            std::vector<std::vector<size_t>> tuples {{}};
            for (const auto &indices: choices) {
                std::vector<std::vector<size_t>> next;
                for (const auto &tuple: tuples) {
                    for (const auto index: indices) {
                        next.push_back(tuple);
                        next.back().push_back(index);
                    }
                }
                tuples = std::move(next);
            }
            const size_t before {covered.size()};
            covered.insert(tuples.begin(), tuples.end());
            if (debug_sync_closed) {
                std::cout << "[sync-closed]     product " << productName(*elt) << " represents "
                          << tuples.size() << " product(s), " << covered.size() - before << " new\n";
            }
        }
        const size_t result {covered.size()};

        if (debug_sync_closed) {
            std::cout << "[sync-closed]     result = " << result << ", total_products = " << total_products
                      << " => " << (total_products > result ? "sync-closed" : "NOT sync-closed") << "\n";
        }
        // Some product of terminal SCCs of the other modules is covered by no sync arc
        if (total_products > result) {
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
void ModuleSS::markOthersCanLoop(const MetaState *ms) {
    if (!ms->hasLocalCycle()) return;
    // All the names of ms: its own one and the ones of the metastates merged into it
    vector<ProductSCC *> names {ms->getMergedNames()};
    names.push_back(ms->getProductSCCName());
    for (size_t module {}; module < ModularPetriNet::getModuleCount(); ++module) {
        if (module == m_id_module) continue;
        for (const auto name: names) {
            const auto found_ms {getParent()->getModule(module)->findMetaStateByProductSCC(*name)};
            if (!found_ms) continue;
            for (const auto &scc: *found_ms->getListSCCs()) {
                scc->setOthersCanLoop(true);
            }
        }
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
