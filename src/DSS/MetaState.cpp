/*
 * MetaState.cpp
 *
 *  Created on: 17 sept. 2015
 *      Author: LIP2
 */
#include <algorithm>
#include <unordered_map>
#include "DSS/MetaState.h"
#include "DSS/ArcSync.h"
#include "DSS/ModuleSS.h"

MetaState::MetaState(LocalGraph *graph) : m_graph(graph) {
    m_id = m_counter++;
}

SCC *MetaState::findSCC(Marking *state) const {
    // A marking of the local graph of this metastate knows its SCC
    if (SCC *scc {state->getSCCContainer()}; scc && scc->getParent() == m_graph) return scc;
    // Otherwise, the marking of this metastate with the same tokens
    const Marking *same {existMarking(state)};
    return same ? same->getSCCContainer() : nullptr;
}

ProductSCC *MetaState::getProductSCCName() const {
    return m_name;
}

void MetaState::setProductSCCName(ProductSCC *name) {
    m_name = name;
    if (ModuleSS *module {getParent()}; module) module->indexName(this, name);
}


bool MetaState::representsProductSCC(const ProductSCC &productscc) const {
    // The (cached) hashes are compared first: the full comparison is only done when they are equal
    const size_t hash {productscc.hash()};
    if (m_name && m_name->hash() == hash && *m_name == productscc) return true;
    return std::any_of(m_merged_names.begin(), m_merged_names.end(), [&productscc, hash](const ProductSCC *name) {
        return name->hash() == hash && *name == productscc;
    });
}

void MetaState::absorbNames(const MetaState &merged) {
    const size_t first_new {m_merged_names.size()};
    if (merged.m_name) m_merged_names.emplace_back(merged.m_name);
    m_merged_names.insert(m_merged_names.end(), merged.m_merged_names.begin(), merged.m_merged_names.end());
    if (ModuleSS *module {getParent()}; module) {
        for (size_t i {first_new}; i < m_merged_names.size(); ++i) module->indexName(this, m_merged_names[i]);
    }
}

MetaState::~MetaState() = default;

MetaState *MetaState::getRepresentative() {
    MetaState *representative {this};
    while (representative->m_merged_into) representative = representative->m_merged_into;
    // Path compression
    for (MetaState *ms {this}; ms != representative;) {
        MetaState *next {ms->m_merged_into};
        ms->m_merged_into = representative;
        ms = next;
    }
    return representative;
}

void MetaState::clearSyncArcs() {
    m_sync_arcs.clear();
}

void MetaState::addSyncArc(unique_ptr<ArcSync> sync_arc) {
    sync_arc->setParent(this);
    m_sync_arcs.push_back(std::move(sync_arc));
}

size_t MetaState::compactSyncArcs() {
    const size_t module {m_id_module};
    struct Hash {
        size_t module;
        size_t operator()(const ArcSync *arc) const {
            const SCC *scc {arc->getSourceSCC(module)};
            size_t hash {scc ? scc->contentHash() : 0x9e3779b97f4a7c15ULL};
            hash = (hash ^ reinterpret_cast<std::uintptr_t>(arc->getFusion())) * 1099511628211ULL;
            return (hash ^ reinterpret_cast<std::uintptr_t>(arc->getDestination())) * 1099511628211ULL;
        }
    };
    struct Equal {
        size_t module;
        bool operator()(const ArcSync *a, const ArcSync *b) const {
            if (a->getFusion() != b->getFusion() || a->getDestination() != b->getDestination()) return false;
            const SCC *scc_a {a->getSourceSCC(module)}, *scc_b {b->getSourceSCC(module)};
            return scc_a == scc_b || (scc_a && scc_b && scc_a->isEqual(*scc_b));
        }
    };
    for (const auto &arc: m_sync_arcs) arc->compact(module);
    std::unordered_set<const ArcSync *, Hash, Equal> seen(m_sync_arcs.size(), Hash {module}, Equal {module});
    const size_t before {m_sync_arcs.size()};
    std::erase_if(m_sync_arcs, [&seen](const unique_ptr<ArcSync> &arc) { return !seen.insert(arc.get()).second; });
    m_sync_arcs.shrink_to_fit();
    return before - m_sync_arcs.size();
}

const vector<unique_ptr<ArcSync>> &MetaState::getSyncSucc() const {
    return m_sync_arcs;
}

uint32_t MetaState::getId() const {
    return m_id;
}

uint32_t MetaState::m_counter {0};
