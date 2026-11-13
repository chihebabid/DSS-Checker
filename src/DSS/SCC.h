#ifndef SCC_H
#define SCC_H
#include <cstdint>
#include <utility>
#include <vector>
#include "DSS/ChildOf.h"
#include "DSS/Marking.h"
#include "DSS/MetaState.h"


class MetaState;

// This class represents a Strongly Connected Component;
// its parent is the metastate it belongs to (set when the metastate computes its SCCs)
class SCC : public ChildOf<MetaState> {
public:
    SCC() = default;
    virtual ~SCC()=default;
    size_t getCount() const;
    void addState(Marking *m);
    std::vector<Marking *> *getListStates();
    // True if both SCCs contain the same markings (they may belong to distinct metastates).
    // The SCC of a marking only depends on the marking, so one common marking and the same size are enough
    bool isEqual(const SCC &scc) const;
    // Hash of the markings of the SCC (independent of their order), consistent with isEqual()
    [[nodiscard]] size_t contentHash() const;
    void setSyncClosed(bool v) {
        m_sync_closed=v;
    }

    bool isSyncClosed() const {
        return m_sync_closed;
    }

    // Marked when another module, in its metastate of the same name, has a cycle of local transitions:
    // it may loop forever, so the module may stay forever in this SCC (executions without fairness)
    void setOthersCanLoop(bool v) {
        m_others_can_loop = v;
    }

    [[nodiscard]] bool othersCanLoop() const {
        return m_others_can_loop;
    }

    // True if no local transition leaves the SCC. Computed once by MetaState::computeSCCTarjan()
    // (synchronisations are not local transitions: they are not taken into account)
    [[nodiscard]] bool isTerminal() const {
        return m_terminal;
    }

    void setTerminal(bool v) {
        m_terminal = v;
    }
private:
    std::vector<Marking *> m_list;
    bool m_sync_closed {};
    bool m_others_can_loop {};
    bool m_terminal {};
    mutable size_t m_hash {};          // Cache of contentHash()
    mutable bool m_hash_valid {false};
};

#endif // SCC_H
