#ifndef SCC_H
#define SCC_H

#include "Marking.h"
#include "MetaState.h"


// This class represents a Strongly Connected Component
class MetaState;

class SCC {
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
    Marking *existState(Marking *m);
    long getCount();
    void addState(Marking *m);
    vector<Marking *> *getListStates();
    bool isEqual(const SCC &scc);
    uint32_t getId() const;
    MetaState * getMetaState() const;
private:
    vector<Marking *> m_list;
    static uint32_t mCounter;
    uint32_t mId;
    MetaState *m_parentMetaState;
};

#endif // SCC_H
