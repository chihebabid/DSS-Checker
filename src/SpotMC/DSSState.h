//
// Created by chiheb on 04/10/23.
//

#ifndef DISTRIBUTEDSTATESPACE_DSSSTATE_H
#define DISTRIBUTEDSTATESPACE_DSSSTATE_H

#include <functional>
#include <twa/twa.hh>
#include "DSS/Marking.h"

// A state of a DSSKripke: a marking of a metastate of the module
// (markings are unique objects inside a metastate: they are compared by address)
class DSSState : public spot::state {
    Marking *m_marking;
public:
    explicit DSSState(Marking *marking) : m_marking(marking) {
    }

    DSSState *clone() const override {
        return new DSSState(m_marking);
    }

    size_t hash() const override {
        return std::hash<const Marking *>{}(m_marking);
    }

    int compare(const spot::state *other) const override {
        const auto o {static_cast<const DSSState *>(other)->m_marking};
        return std::less<const Marking *>{}(m_marking, o) ? -1 : (m_marking != o);
    }

    [[nodiscard]] Marking *getMarking() const {
        return m_marking;
    }
};


#endif //DISTRIBUTEDSTATESPACE_DSSSTATE_H
