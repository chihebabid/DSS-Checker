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
    struct IteratorSucc {
        explicit IteratorSucc(SCC * ptr):m_ptr(ptr){
        }

        std::vector<std::pair<SCC * ,Transition*>>::iterator begin() {
            update();
            return m_succ.begin();
        }
        std::vector<std::pair<SCC * ,Transition*>>::iterator end() { return m_succ.end(); }
        std::vector<std::pair<SCC * ,Transition*>>& getSuccessors() {
            update();
            return m_succ;
        }
    private:
        void update();
        SCC *m_ptr;
        std::vector<std::pair<SCC *,Transition*>> m_succ;
    };

public:
    SCC();
    virtual ~SCC()=default;
    Marking *findMarking(Marking *m);
    size_t getCount() const;
    void addState(Marking *m);
    std::vector<Marking *> *getListStates();
    bool isEqual(const SCC &scc);
    [[nodiscard]] uint32_t getId() const;
    std::vector<std::pair<SCC * ,Transition*>>::iterator beginSucc() {
        return m_iterator_succ.begin();
    }
    std::vector<std::pair<SCC * ,Transition*>>::iterator endSucc() {
        return m_iterator_succ.end();
    }
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
    bool isTerminal() const;
private:
    std::vector<Marking *> m_list;
    static uint32_t m_counter;
    uint32_t m_id;
    IteratorSucc m_iterator_succ {this};
    bool m_sync_closed {};
    bool m_others_can_loop {};
};

#endif // SCC_H
