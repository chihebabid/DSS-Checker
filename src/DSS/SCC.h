#ifndef SCC_H
#define SCC_H
#include <cstdint>
#include <utility>
#include <vector>
#include "DSS/ChildOf.h"
#include "DSS/Marking.h"
#include "DSS/LocalGraph.h"

// This class represents a Strongly Connected Component of the local transitions;
// its parent is the local graph it belongs to (set when the graph computes its SCCs). It is shared by the metastates
// having this local graph: what depends on a metastate (sync-closure) is stored in the metastate
class SCC : public ChildOf<LocalGraph> {
public:
    SCC() = default;
    virtual ~SCC()=default;
    size_t getCount() const;
    void addState(Marking *m);
    std::vector<Marking *> *getListStates();
    // True if both SCCs contain the same markings (they may belong to distinct local graphs).
    // The SCC of a marking only depends on the marking, so one common marking and the same size are enough
    bool isEqual(const SCC &scc) const;
    // Hash of the markings of the SCC (independent of their order), consistent with isEqual()
    [[nodiscard]] size_t contentHash() const;
    // True if no local transition leaves the SCC. Computed once by LocalGraph::computeSCCTarjan()
    // (synchronisations are not local transitions: they are not taken into account)
    [[nodiscard]] bool isTerminal() const {
        return m_terminal;
    }

    void setTerminal(bool v) {
        m_terminal = v;
    }
private:
    std::vector<Marking *> m_list;
    bool m_terminal {};
    mutable size_t m_hash {};          // Cache of contentHash()
    mutable bool m_hash_valid {false};
};

#endif // SCC_H
