#ifndef PELEMENT_DSS_H
#define PELEMENT_DSS_H
#define ListLocalStates vector<Marking*>*
#define ListGlobalStates vector<ListLocalStates>*

#include "Fusion.h"

class PElement_dss {
public:
    PElement_dss();

    PElement_dss(const PElement_dss &elt) {
        this->m_fusion = elt.m_fusion;
        this->m_gs = elt.m_gs;
    }

    virtual PElement_dss &operator=(const PElement_dss &elt) {
        this->m_gs = elt.m_gs;
        this->m_fusion = elt.m_fusion;
        return *this;
    }

    virtual ~PElement_dss();

    ListGlobalStates m_gs;
    Fusion *m_fusion;
};

#endif // PELEMENT_DSS_H
