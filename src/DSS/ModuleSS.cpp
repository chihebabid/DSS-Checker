//
// Created by chiheb on 13/07/22.
//
#include <algorithm>
#include <vector>
#include <utility>
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
    ms->setRank(m_next_rank++);
    // Names it may already have
    indexName(ms, ms->getProductSCCName());
    for (const auto name: ms->getMergedNames()) indexName(ms, name);
    return true;
}

void ModuleSS::indexName(MetaState *ms, const ProductSCC *name) {
    if (name && contains(ms)) m_name_index.emplace(name->hash(), ms);
}

void ModuleSS::setInitialState(const ProductSCC *name, const Marking &marking) {
    m_initial_name = name;
    m_initial_marking = std::make_unique<Marking>(marking);
}

Marking *ModuleSS::getInitialMarking() {
    MetaState *ms {getInitialMetaState()};
    return ms ? ms->existMarking(m_initial_marking.get()) : nullptr;
}

MetaState *ModuleSS::getInitialMetaState() {
    return findMetaStateByProductSCC(*const_cast<ProductSCC *>(m_initial_name));
}

MetaState *ModuleSS::findMetaStateByProductSCC(ProductSCC &productscc) {
    MetaState *found {nullptr};
    const auto [first, last] {m_name_index.equal_range(productscc.hash())};
    for (auto it {first}; it != last; ++it) {
        MetaState *candidate {it->second};
        if (!contains(candidate)) continue; // merged on the fly
        if ((!found || candidate->getRank() < found->getRank()) && candidate->representsProductSCC(productscc)) {
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

std::vector<std::pair<std::uintptr_t, std::uintptr_t>> ModuleSS::syncSignature(MetaState *ms) const {
    std::vector<std::pair<std::uintptr_t, std::uintptr_t>> signature;
    signature.reserve(ms->getSyncSucc().size());
    for (const auto &arc: ms->getSyncSucc()) {
        MetaState *dest {arc->getDestination()->getRepresentative()};
        signature.emplace_back(reinterpret_cast<std::uintptr_t>(arc->getFusion()),
                               dest == ms ? 0 : reinterpret_cast<std::uintptr_t>(dest));
    }
    std::sort(signature.begin(), signature.end());
    signature.erase(std::unique(signature.begin(), signature.end()), signature.end());
    return signature;
}

MetaState *ModuleSS::mergeIfEquivalent(MetaState *ms) {
    const auto signature {syncSignature(ms)};
    size_t key {ms->contentHash()};
    for (const auto &[fusion, dest]: signature) {
        key = (key ^ fusion) * 1099511628211ULL;
        key = (key ^ dest) * 1099511628211ULL;
    }
    const auto [first, last] {m_complete_index.equal_range(key)};
    for (auto it {first}; it != last; ++it) {
        MetaState *candidate {it->second};
        if (candidate == ms || !contains(candidate) || !(*candidate == *ms)) continue;
        if (syncSignature(candidate) != signature) continue;
        // As in the final reduction, the first inserted metastate represents the other one
        MetaState *kept {candidate->getRank() < ms->getRank() ? candidate : ms};
        MetaState *merged {kept == ms ? candidate : ms};
        kept->absorbNames(*merged); // also indexes the names in this module
        merged->setMergedInto(kept);
        merged->clearSyncArcs();
        merged->setParent(nullptr);
        it->second = kept;
        ++m_merged_count;
        return kept;
    }
    m_complete_index.emplace(key, ms);
    return nullptr;
}

size_t ModuleSS::finishMerges() {
    m_complete_index = {};
    if (m_merged_count == 0) return 0;
    std::erase_if(m_metastates, [this](const MetaState *ms) { return !contains(ms); });
    std::erase_if(m_name_index, [this](const auto &entry) { return !contains(entry.second); });
    for (const auto ms: m_metastates) {
        for (const auto &arc: ms->getSyncSucc()) arc->setDestination(arc->getDestination()->getRepresentative());
    }
    return std::exchange(m_merged_count, 0);
}

size_t ModuleSS::reduceByBisimulation() {
    const size_t n {m_metastates.size()};
    if (n < 2) return 0;
    std::unordered_map<const MetaState *, uint32_t> position;
    position.reserve(n);
    for (uint32_t i {}; i < n; ++i) position.emplace(m_metastates[i], i);
    // Sync arcs of each metastate: (fusion, position of the destination), in the order of getSyncSucc()
    std::vector<std::vector<std::pair<const TransitionFusionSet *, uint32_t>>> arcs(n);
    for (uint32_t i {}; i < n; ++i) {
        for (const auto &arc: m_metastates[i]->getSyncSucc()) {
            arcs[i].emplace_back(arc->getFusion(), position.at(arc->getDestination()));
        }
    }

    // Initial partition: same markings
    std::vector<uint32_t> block(n);
    size_t block_count {};
    {
        std::unordered_map<size_t, std::vector<uint32_t>> blocks_of_hash; // content hash -> blocks
        std::vector<uint32_t> first_of_block;                           // a metastate of each block
        for (uint32_t i {}; i < n; ++i) {
            auto &candidates {blocks_of_hash[m_metastates[i]->contentHash()]};
            const auto it {std::find_if(candidates.begin(), candidates.end(), [&](const uint32_t b) {
                return *m_metastates[first_of_block[b]] == *m_metastates[i];
            })};
            if (it != candidates.end()) {
                block[i] = *it;
            } else {
                block[i] = static_cast<uint32_t>(block_count++);
                first_of_block.push_back(i);
                candidates.push_back(block[i]);
            }
        }
    }

    // Refinement: the new block of a metastate is given by its block and the set of (fusion, block of the
    // destination) of its arcs; it stops when no block is split any more
    struct SignatureHash {
        size_t operator()(const std::vector<uint64_t> &signature) const {
            size_t hash {signature.size()};
            for (const auto value: signature) hash = (hash ^ value) * 1099511628211ULL;
            return hash;
        }
    };
    std::vector<std::pair<uint64_t, uint64_t>> labels;
    std::vector<uint64_t> signature;
    while (true) {
        std::unordered_map<std::vector<uint64_t>, uint32_t, SignatureHash> new_blocks;
        new_blocks.reserve(block_count * 2);
        std::vector<uint32_t> next(n);
        for (uint32_t i {}; i < n; ++i) {
            labels.clear();
            for (const auto &[fusion, dest]: arcs[i]) {
                labels.emplace_back(reinterpret_cast<uintptr_t>(fusion), block[dest]);
            }
            std::sort(labels.begin(), labels.end());
            labels.erase(std::unique(labels.begin(), labels.end()), labels.end());
            signature.assign(1, block[i]);
            for (const auto &[fusion, dest_block]: labels) {
                signature.push_back(fusion);
                signature.push_back(dest_block);
            }
            next[i] = new_blocks.emplace(signature, static_cast<uint32_t>(new_blocks.size())).first->second;
        }
        block = std::move(next);
        if (new_blocks.size() == block_count) break;
        block_count = new_blocks.size();
    }
    if (block_count == n) return 0;

    // Representative of each block: its first inserted metastate
    std::vector<MetaState *> representative(block_count, nullptr);
    for (uint32_t i {}; i < n; ++i) {
        if (!representative[block[i]]) representative[block[i]] = m_metastates[i];
    }
    // Redirect the arcs to the representatives
    for (uint32_t i {}; i < n; ++i) {
        const auto &sync_arcs {m_metastates[i]->getSyncSucc()};
        for (size_t k {}; k < sync_arcs.size(); ++k) {
            MetaState *target {representative[block[arcs[i][k].second]]};
            if (sync_arcs[k]->getDestination() != target) sync_arcs[k]->setDestination(target);
        }
    }
    // Merge the other metastates into their representative. They stay alive (their SCCs are still referenced
    // by product SCCs), but they lose their arcs and leave the module
    size_t removed {};
    for (uint32_t i {}; i < n; ++i) {
        MetaState *ms {m_metastates[i]};
        MetaState *target {representative[block[i]]};
        if (target == ms) continue;
        target->absorbNames(*ms); // also indexes the names in this module
        ms->clearSyncArcs();
        ms->setParent(nullptr);
        ++removed;
    }
    std::erase_if(m_metastates, [this](const MetaState *ms) { return !contains(ms); });
    std::erase_if(m_name_index, [this](const auto &entry) { return !contains(entry.second); });
    return removed;
}

/*
 * @brief Compute the sync-closure property of the terminal SCCs of all metastates
 */
void ModuleSS::computeSyncClosedSCC(MetaState *ms) {
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
            ms->setSyncClosed(scc);
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
                found_ms->setOthersCanLoop(scc);
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
