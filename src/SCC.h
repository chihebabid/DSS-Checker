#ifndef SCC_H
#define SCC_H

#include "Marking.h"

// This class represents a Strongly Connected Component
class SCC {
public:
    SCC();
    virtual ~SCC();
    Marking *existState(Marking *m);
    long getCount();
    void addState(Marking *m);
    vector<Marking *> *getListStates();
    bool isEqual(const SCC &scc);


    uint32_t getId() const;

private:
    vector<Marking *> m_list;
    static uint32_t mCounter;
    uint32_t mId;
};

#endif // SCC_H
