#ifndef SCC_H
#define SCC_H

#include "DSS/Marking.h"
#include "DSS/MetaState.h"

// This class represents a Strongly Connected Component
class MetaState;
class SCC {
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
