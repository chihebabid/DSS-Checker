#ifndef DISTRIBUTEDSTATESPACE_LOCALGRAPH_H
#define DISTRIBUTEDSTATESPACE_LOCALGRAPH_H

#include <cstddef>
#include <cstdint>
#include <unordered_set>
#include <vector>
#include "DSS/Marking.h"

class SCC;

/*
 * The markings of a metastate and their SCCs: the graph of the local transitions reached from a marking.
 * It only depends on its markings, so it is shared by all the metastates having the same markings (they differ by
 * their names, their sync arcs and the sync-closure of their SCCs, see MetaState). It is built by
 * PetriNet::getMetaState(), which owns it: a local graph lives until the end, so that the SCCs referenced by the
 * names of the metastates remain valid.
 */
class LocalGraph {
public:
    LocalGraph() = default;
    // Destroys its markings and its SCCs
    ~LocalGraph();
    LocalGraph(const LocalGraph &) = delete;
    LocalGraph &operator=(const LocalGraph &) = delete;

    [[nodiscard]] const std::vector<Marking *> &getListMarkings() const { return m_nodes; }
    // Its SCCs, in the order of Tarjan's algorithm
    [[nodiscard]] const std::vector<SCC *> &getListSCCs() const { return m_sccs; }
    // SCC of the first marking (the one the graph has been computed from)
    [[nodiscard]] SCC *getInitialSCC() const;
    // Returns the marking of this graph equal to *marking, nullptr if there is none
    [[nodiscard]] Marking *existMarking(const Marking *marking) const;
    // Inserts m unless an equal marking exists: returns that marking, nullptr if m has been inserted
    Marking *insertMarking(Marking *m);
    // Computes the SCCs (iterative Tarjan) and marks the terminal ones; called once all the markings are inserted
    void computeSCCTarjan();
    [[nodiscard]] size_t getTerminalSCCsCount() const;
    // True if the graph contains a cycle of local transitions: an SCC with several markings, or a loop
    [[nodiscard]] bool hasLocalCycle() const;
    // Same markings (the order does not matter); the content hashes are compared first
    bool operator==(const LocalGraph &graph) const;
    // Hash of the markings (independent of their order), consistent with operator==; computed once
    [[nodiscard]] size_t contentHash() const;
    // Number of local transitions
    [[nodiscard]] uint32_t getArcCount() const;
private:
    std::vector<Marking *> m_nodes; // In insertion order
    std::unordered_set<Marking *, Marking::PtrHash, Marking::PtrEqual> m_nodes_index; // Same markings, fast lookup
    std::vector<SCC *> m_sccs;
    mutable size_t m_content_hash {};
    mutable bool m_content_hash_valid {false};
};

#endif //DISTRIBUTEDSTATESPACE_LOCALGRAPH_H
