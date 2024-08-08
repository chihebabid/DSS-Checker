//
// Created by chiheb on 04/10/23.
//

#ifndef DISTRIBUTEDSTATESPACE_DSSSTATE_H
#define DISTRIBUTEDSTATESPACE_DSSSTATE_H

#include <twa/twa.hh>
#include "DSS/SCC.h"
#include "DSS/MetaState.h"

class DSSState : public spot::state {
    SCC *m_scc;
public:
    DSSState(SCC* scc) : m_scc(scc) {

    };

    DSSState* clone() const override
    {
        return new DSSState(m_scc);
    }

    size_t hash() const override
    {
        return m_scc->getId();
    }

    int compare(const spot::state* other) const override
    {
        auto o = static_cast<const DSSState*>(other);
        size_t oh = o->hash();
        size_t h = hash();
        if (h < oh)
            return -1;
        else
            return h > oh;
    }

    SCC * getSCC() const {
        return m_scc;
    }
};


#endif //DISTRIBUTEDSTATESPACE_DSSSTATE_H
