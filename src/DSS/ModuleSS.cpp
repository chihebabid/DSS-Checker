//
// Created by chiheb on 13/07/22.
//
#include <algorithm>
#include <vector>
#include <iostream>
#include "DSS/ModuleSS.h"

#include "DistributedSS.h"
#include "DSS/MetaState.h"
#include "Petri/ModularPetriNet.h"

// Set to false to remove the trace of computeSyncClosedSCC()
static constexpr bool debug_sync_closed {false};

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

/*
 * Try to insert a new MetaState
 * @return : false if it is already exists
 */
bool ModuleSS::insertMS(MetaState *ms) {
    ms->setIdModule(m_id_module);
    ms->setParent(this);
    m_metastates.push_back(ms);
    m_rank[ms] = m_next_rank++;
    // Names it may already have
    indexName(ms, ms->getProductSCCName());
    for (const auto name: ms->getMergedNames()) indexName(ms, name);
    return true;
}

void ModuleSS::indexName(MetaState *ms, const ProductSCC *name) {
    if (name && m_rank.contains(ms)) m_name_index.emplace(name->hash(), ms);
}

MetaState *ModuleSS::findMetaStateByProductSCC(ProductSCC &productscc) {
    MetaState *found {nullptr};
    const auto [first, last] {m_name_index.equal_range(productscc.hash())};
    for (auto it {first}; it != last; ++it) {
        MetaState *candidate {it->second};
        if ((!found || m_rank.at(candidate) < m_rank.at(found)) && candidate->representsProductSCC(productscc)) {
            found = candidate;
        }
    }
    return found;
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
    std::erase_if(m_name_index, [ms](const auto &entry) { return entry.second == ms; });
    m_rank.erase(ms);

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
    std::vector<std::vector<SCC *>> terminal_sccs_of(module_count); // Terminal SCCs of each other module
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

        // result: number of distinct products of terminal SCCs of the other modules covered by the sync arcs
        // leaving scc (union: a product covered by several arcs is counted once).
        // A product (tuple of indices i_j in terminal_sccs_of[j]) is coded by the mixed-radix number
        // i_j1 + n_j1 * (i_j2 + n_j2 * (...)), n_j being the number of terminal SCCs of module j
        std::vector<bool> covered(total_products, false);
        size_t result{};
        auto max_products {getMaxProductOfTerminalSCCs(ms, scc)};
        if (debug_sync_closed) {
            std::cout << "[sync-closed]   SCC " << sccName(scc, m_id_module) << ": terminal, "
                      << max_products.size() << " maximal product(s)\n";
        }
        for (const auto &elt : max_products) {
            if (result == total_products) break; // Everything is already covered
            // Codes of the products represented by elt, built module by module
            std::vector<size_t> codes {0};
            size_t radix {1};
            for (size_t module{}; module < module_count; ++module) {
                if (module == m_id_module) continue;
                const auto &terminals {terminal_sccs_of[module]};
                // Terminal SCCs of the module allowed by elt: all of them for "-", else the one equal to its SCC
                // (compared by content: the SCC of elt may belong to a merged metastate)
                std::vector<size_t> indices;
                for (size_t i{}; i < terminals.size(); ++i) {
                    if (elt->isAny(module) || terminals[i]->isEqual(*elt->getSCC(module))) indices.push_back(i);
                }
                std::vector<size_t> next;
                next.reserve(codes.size() * indices.size());
                for (const auto code: codes) {
                    for (const auto i: indices) next.push_back(code + i * radix);
                }
                codes = std::move(next);
                radix *= terminals.size();
            }
            size_t new_products{};
            for (const auto code: codes) {
                if (!covered[code]) {
                    covered[code] = true;
                    ++new_products;
                }
            }
            result += new_products;
            if (debug_sync_closed) {
                std::cout << "[sync-closed]     product " << productName(*elt) << " represents "
                          << codes.size() << " product(s), " << new_products << " not covered yet\n";
            }
        }

        if (debug_sync_closed) {
            std::cout << "[sync-closed]     result = " << result << ", total_products = " << total_products
                      << " => " << (total_products > result ? "sync-closed" : "NOT sync-closed") << "\n";
        }
        // Some product of terminal SCCs of the other modules is covered by no sync arc
        if (total_products > result) {
            scc->setSyncClosed(true);
        }
    }
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
