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

MetaState::MetaState() {
    m_id = m_counter++;
}

vector<Marking *> MetaState::getListMarkings()  const {
    return m_nodes;
}

/*vector<InternalArc>* MetaState::getListArcs() {
    return m_graph->getListArcs();
}*/

vector<SCC *> *MetaState::getListSCCs() const {
    return &m_sccs;
}

SCC *MetaState::findSCC(Marking *state) {
    for (int i = 0; i < m_sccs.size(); i++) {
        vector<Marking *> *list_states = m_sccs[i]->getListStates();
        if (std::find(list_states->begin(), list_states->end(), state) != list_states->end()) return m_sccs.at(i);
    }
    return nullptr;
}

SCC *MetaState::getInitialSCC() {
    return findSCC(m_nodes[0]);
}


ProductSCC *MetaState::getProductSCCName() const {
    return m_name;
}

void MetaState::setProductSCCName(ProductSCC *name) {
    m_name = name;
}


bool MetaState::representsProductSCC(const ProductSCC &productscc) const {
    if (m_name && *m_name == productscc) return true;
    return std::any_of(m_merged_names.begin(), m_merged_names.end(),
                       [&productscc](const ProductSCC *name) { return *name == productscc; });
}

void MetaState::absorbNames(const MetaState &merged) {
    if (merged.m_name) m_merged_names.emplace_back(merged.m_name);
    m_merged_names.insert(m_merged_names.end(), merged.m_merged_names.begin(), merged.m_merged_names.end());
}

MetaState::~MetaState() = default;

void MetaState::removeSyncArcsTo(const MetaState *destination) {
    std::erase_if(m_sync_arcs, [destination](const unique_ptr<ArcSync> &arc) {
        return arc->getDestination() == destination;
    });
}

void MetaState::clearSyncArcs() {
    m_sync_arcs.clear();
}

void MetaState::addSyncArc(unique_ptr<ArcSync> sync_arc) {
    sync_arc->setParent(this);
    m_sync_arcs.push_back(std::move(sync_arc));
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

bool MetaState::operator==(MetaState &ms) const {
    if (ms.m_nodes.size() != m_nodes.size()) return false;
    // Markings are unique inside a metastate: same count and inclusion mean equality
    return std::all_of(ms.m_nodes.begin(), ms.m_nodes.end(),
                       [this](const Marking *marking) { return existMarking(marking) != nullptr; });
}

uint32_t MetaState::getId() const {
    return m_id;
}

uint32_t MetaState::m_counter {0};

// Working data of Tarjan algorithm, only alive during computeSCCTarjan()
struct MetaState::TarjanContext {
    struct NodeInfo {
        int index {-1}; // -1: not visited yet
        int lowlink {0};
        bool on_stack {false};
    };
    unordered_map<const Marking *, NodeInfo> infos;
    int next_index {0};
    vector<Marking *> stack;
};

void MetaState::computeStrongConnectedComponents(Marking *v, TarjanContext &ctx) {
    auto &info_v {ctx.infos[v]}; // references stay valid: unordered_map never moves its elements
    info_v.index = ctx.next_index;
    info_v.lowlink = ctx.next_index;
    ctx.next_index++;
    ctx.stack.push_back(v);
    info_v.on_stack = true;

    // Consider successors of v
    for (const auto &elt: v->getListSucc()) {
        Marking *w = elt.second;
        auto &info_w {ctx.infos[w]};
        if (info_w.index == -1) {
            computeStrongConnectedComponents(w, ctx);
            info_v.lowlink = min(info_v.lowlink, info_w.lowlink);
        } else if (info_w.on_stack) info_v.lowlink = min(info_v.lowlink, info_w.index);
    }
    // If v is a root node, pop the stack and generate an SCC
    if (info_v.lowlink == info_v.index) {
        SCC *scc = new SCC();
        scc->setParent(this);
        Marking *w;
        do {
            w = ctx.stack.back();
            ctx.stack.pop_back();
            ctx.infos[w].on_stack = false;
            scc->addState(w);
        } while (w != v);
        m_sccs.emplace_back(scc);
    }
}

Marking *MetaState::existMarking(const Marking *marking) const {
    // The lookup only reads *marking (hash and comparison)
    const auto it {m_nodes_index.find(const_cast<Marking *>(marking))};
    return it == m_nodes_index.end() ? nullptr : *it;
}

Marking *MetaState::insertMarking(Marking *m) {
    const auto [it, inserted] {m_nodes_index.insert(m)};
    if (!inserted) return *it;
    m_nodes.push_back(m);
    return nullptr;
}


void MetaState::computeSCCTarjan() {
    TarjanContext ctx;
    ctx.infos.reserve(m_nodes.size());
    for (auto &node: m_nodes) {
        if (ctx.infos[node].index == -1) computeStrongConnectedComponents(node, ctx);
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
