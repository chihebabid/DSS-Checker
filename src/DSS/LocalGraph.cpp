#include <algorithm>
#include "DSS/LocalGraph.h"
#include "DSS/SCC.h"

LocalGraph::~LocalGraph() {
    for (const auto scc: m_sccs) delete scc;
    for (const auto marking: m_nodes) delete marking;
}

SCC *LocalGraph::getInitialSCC() const {
    return m_nodes[0]->getSCCContainer();
}

Marking *LocalGraph::existMarking(const Marking *marking) const {
    // The lookup only reads *marking (hash and comparison)
    const auto it {m_nodes_index.find(const_cast<Marking *>(marking))};
    return it == m_nodes_index.end() ? nullptr : *it;
}

Marking *LocalGraph::insertMarking(Marking *m) {
    const auto [it, inserted] {m_nodes_index.insert(m)};
    if (!inserted) return *it;
    m->setPosition(static_cast<uint32_t>(m_nodes.size()));
    m_nodes.push_back(m);
    m_content_hash_valid = false;
    return nullptr;
}

/*
 * Tarjan algorithm, iterative (no recursion: no stack overflow on large graphs).
 * Markings are identified by their position in m_nodes (Marking::getPosition()). The SCCs, and the markings inside each
 * SCC, are produced in the same order as by the recursive version.
 * Then, the terminal SCCs are marked: no local transition leaves them.
 */
void LocalGraph::computeSCCTarjan() {
    const size_t n {m_nodes.size()};

    std::vector<int> index(n, -1), lowlink(n, 0);
    std::vector<char> on_stack(n, 0);
    std::vector<uint32_t> stack;
    struct Frame {
        uint32_t node;
        size_t next_succ; // Next successor of node to consider
    };
    std::vector<Frame> call_stack;
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
                else if (on_stack[w]) lowlink[v] = std::min(lowlink[v], index[w]);
                continue;
            }
            // All the successors of v have been considered
            call_stack.pop_back();
            if (!call_stack.empty()) {
                const uint32_t parent {call_stack.back().node};
                lowlink[parent] = std::min(lowlink[parent], lowlink[v]);
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

size_t LocalGraph::getTerminalSCCsCount() const {
    return std::count_if(m_sccs.begin(), m_sccs.end(), [](const SCC *scc) {
        return scc->isTerminal();
    });
}

bool LocalGraph::hasLocalCycle() const {
    return std::any_of(m_sccs.begin(), m_sccs.end(), [](SCC *scc) {
        if (scc->getCount() > 1) return true;
        Marking *marking {scc->getListStates()->at(0)};
        const auto &succ {marking->getListSucc()};
        return std::any_of(succ.begin(), succ.end(), [marking](const auto &elt) { return elt.second == marking; });
    });
}

size_t LocalGraph::contentHash() const {
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

bool LocalGraph::operator==(const LocalGraph &graph) const {
    if (this == &graph) return true;
    if (graph.m_nodes.size() != m_nodes.size()) return false;
    if (graph.contentHash() != contentHash()) return false;
    // Markings are unique inside a graph: same count and inclusion mean equality
    return std::all_of(graph.m_nodes.begin(), graph.m_nodes.end(),
                       [this](const Marking *marking) { return existMarking(marking) != nullptr; });
}

uint32_t LocalGraph::getArcCount() const {
    uint32_t res {};
    for (const auto marking: m_nodes) res += marking->getListSucc().size();
    return res;
}
