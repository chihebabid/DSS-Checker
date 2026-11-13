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

MetaState::MetaState() {
    m_id = m_counter++;
}

const vector<Marking *> &MetaState::getListMarkings() const {
    return m_nodes;
}

vector<SCC *> *MetaState::getListSCCs() const {
    return &m_sccs;
}

SCC *MetaState::findSCC(Marking *state) const {
    // A marking of this metastate knows its SCC
    if (SCC *scc {state->getSCCContainer()}; scc && scc->getParent() == this) return scc;
    // Otherwise, the marking of this metastate with the same tokens
    const Marking *same {existMarking(state)};
    return same ? same->getSCCContainer() : nullptr;
}

SCC *MetaState::getInitialSCC() const {
    return m_nodes[0]->getSCCContainer();
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

MetaState::~MetaState() {
    for (const auto scc: m_sccs) delete scc;
    for (const auto marking: m_nodes) delete marking;
}

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

size_t MetaState::removeDuplicateSyncArcs() {
    struct Hash {
        size_t operator()(const ArcSync *arc) const {
            size_t hash {arc->getSourceProduct().hash()};
            hash = (hash ^ reinterpret_cast<std::uintptr_t>(arc->getFusion())) * 1099511628211ULL;
            return (hash ^ reinterpret_cast<std::uintptr_t>(arc->getDestination())) * 1099511628211ULL;
        }
    };
    struct Equal {
        bool operator()(const ArcSync *a, const ArcSync *b) const {
            return a->getFusion() == b->getFusion() && a->getDestination() == b->getDestination()
                   && a->getSourceProduct() == b->getSourceProduct();
        }
    };
    std::unordered_set<const ArcSync *, Hash, Equal> seen;
    seen.reserve(m_sync_arcs.size());
    const size_t before {m_sync_arcs.size()};
    std::erase_if(m_sync_arcs, [&seen](const unique_ptr<ArcSync> &arc) { return !seen.insert(arc.get()).second; });
    return before - m_sync_arcs.size();
}

const vector<unique_ptr<ArcSync>> &MetaState::getSyncSucc() const {
    return m_sync_arcs;
}

uint32_t MetaState::getArcCount() {
    u_int32_t res = 0;
    for (auto elt: m_nodes)
        res += elt->getListSucc().size();
    return res;
}

size_t MetaState::contentHash() const {
    if (!m_content_hash_valid) {
        // Sum of mixed hashes: independent of the order of the markings
        size_t hash {m_nodes.size()};
        for (const auto marking: m_nodes) {
            size_t h {marking->hash()};
            h ^= h >> 33;
            h *= 0xff51afd7ed558ccdULL;
            h ^= h >> 33;
            hash += h;
        }
        m_content_hash = hash;
        m_content_hash_valid = true;
    }
    return m_content_hash;
}

bool MetaState::operator==(const MetaState &ms) const {
    if (ms.m_nodes.size() != m_nodes.size()) return false;
    if (ms.contentHash() != contentHash()) return false;
    // Markings are unique inside a metastate: same count and inclusion mean equality
    return std::all_of(ms.m_nodes.begin(), ms.m_nodes.end(),
                       [this](const Marking *marking) { return existMarking(marking) != nullptr; });
}

uint32_t MetaState::getId() const {
    return m_id;
}

uint32_t MetaState::m_counter {0};

Marking *MetaState::existMarking(const Marking *marking) const {
    // The lookup only reads *marking (hash and comparison)
    const auto it {m_nodes_index.find(const_cast<Marking *>(marking))};
    return it == m_nodes_index.end() ? nullptr : *it;
}

Marking *MetaState::insertMarking(Marking *m) {
    const auto [it, inserted] {m_nodes_index.insert(m)};
    if (!inserted) return *it;
    m->setPosition(static_cast<uint32_t>(m_nodes.size()));
    m_nodes.push_back(m);
    m_content_hash_valid = false;
    return nullptr;
}


/*
 * Tarjan algorithm, iterative (no recursion: no stack overflow on large metastates).
 * Markings are identified by their position in m_nodes (Marking::getPosition()). The SCCs, and the markings inside each SCC,
 * are produced in the same order as by the recursive version.
 * Then, the terminal SCCs are marked: no local transition leaves them.
 */
void MetaState::computeSCCTarjan() {
    const size_t n {m_nodes.size()};

    vector<int> index(n, -1), lowlink(n, 0);
    vector<char> on_stack(n, 0);
    vector<uint32_t> stack;
    struct Frame {
        uint32_t node;
        size_t next_succ; // Next successor of node to consider
    };
    vector<Frame> call_stack;
    int next_index {0};
    const auto visit = [&](const uint32_t v) {
        index[v] = lowlink[v] = next_index++;
        stack.push_back(v);
        on_stack[v] = 1;
        call_stack.push_back({v, 0});
    };

    for (uint32_t root {}; root < n; ++root) {
        if (index[root] != -1) continue;
        visit(root);
        while (!call_stack.empty()) {
            Frame &frame {call_stack.back()};
            const uint32_t v {frame.node};
            const auto &succ {m_nodes[v]->getListSucc()};
            if (frame.next_succ < succ.size()) {
                const uint32_t w {succ[frame.next_succ++].second->getPosition()};
                if (index[w] == -1) visit(w); // frame is no longer valid after this call
                else if (on_stack[w]) lowlink[v] = min(lowlink[v], index[w]);
                continue;
            }
            // All the successors of v have been considered
            call_stack.pop_back();
            if (!call_stack.empty()) {
                const uint32_t parent {call_stack.back().node};
                lowlink[parent] = min(lowlink[parent], lowlink[v]);
            }
            // If v is a root node, pop the stack and generate an SCC
            if (lowlink[v] == index[v]) {
                SCC *scc = new SCC();
                scc->setParent(this);
                uint32_t w;
                do {
                    w = stack.back();
                    stack.pop_back();
                    on_stack[w] = 0;
                    scc->addState(m_nodes[w]);
                } while (w != v);
                m_sccs.emplace_back(scc);
            }
        }
    }

    for (const auto scc: m_sccs) {
        const auto &markings {*scc->getListStates()};
        scc->setTerminal(std::all_of(markings.begin(), markings.end(), [scc](const Marking *marking) {
            const auto &succ {marking->getListSucc()};
            return std::all_of(succ.begin(), succ.end(), [scc](const auto &elt) {
                return elt.second->getSCCContainer() == scc;
            });
        }));
    }
}

size_t MetaState::getTerminalSCCsCount() const {
    return std::count_if(m_sccs.begin(), m_sccs.end(), [](const SCC *scc) {
        return scc->isTerminal();
    });
}

bool MetaState::hasLocalCycle() const {
    return std::any_of(m_sccs.begin(), m_sccs.end(), [](SCC *scc) {
        if (scc->getCount() > 1) return true;
        Marking *marking {scc->getListStates()->at(0)};
        const auto &succ {marking->getListSucc()};
        return std::any_of(succ.begin(), succ.end(), [marking](const auto &elt) { return elt.second == marking; });
    });
}
