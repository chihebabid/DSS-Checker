//
// Created by chiheb on 04/10/23.
//

#ifndef DISTRIBUTEDSTATESPACE_DSSSTATE_H
#define DISTRIBUTEDSTATESPACE_DSSSTATE_H

#include <twa/twa.hh>
#include "../SCC.h"
#include "../MetaState.h"

class DSSState : public spot::state {
    SCC *m_scc;
    MetaState *m_metaState;
    /**
     * Default constructor
     */
public:
    DSSState(SCC* scc,MetaState *ms) : m_scc(scc),m_metaState(ms) {

    };

    DSSState* clone() const override
    {
        return new DSSState(m_scc,m_metaState);
    }

    size_t hash() const override
    {
        return (m_metaState->getId()<<32) | m_scc->getId();
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



};


#endif //DISTRIBUTEDSTATESPACE_DSSSTATE_H
