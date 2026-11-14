//
// Created by chiheb on 04/10/23.
//

#ifndef DISTRIBUTEDSTATESPACE_DSSSTATE_H
#define DISTRIBUTEDSTATESPACE_DSSSTATE_H

#include <functional>
#include <twa/twa.hh>
#include "DSS/Marking.h"

class MetaState;

// A marking of a metastate of the module. The markings are shared by the metastates having the same markings (see
// LocalGraph): the metastate is needed too (its sync arcs and the sync-closure of its SCCs are its own).
// Markings and metastates are unique objects: they are compared by address
struct DSSStateKey {
    MetaState *metastate;
    Marking *marking;

    bool operator==(const DSSStateKey &other) const = default;

    struct Hash {
        size_t operator()(const DSSStateKey &key) const {
            const size_t h {std::hash<const void *>{}(key.metastate)};
            return (h ^ (h >> 31)) * 1099511628211ULL ^ std::hash<const void *>{}(key.marking);
        }
    };
};

// A state of a DSSKripke
class DSSState : public spot::state {
    DSSStateKey m_key;
public:
    explicit DSSState(const DSSStateKey &key) : m_key(key) {
    }

    DSSState *clone() const override {
        return new DSSState(m_key);
    }

    size_t hash() const override {
        return DSSStateKey::Hash{}(m_key);
    }

    int compare(const spot::state *other) const override {
        const auto &o {static_cast<const DSSState *>(other)->m_key};
        if (m_key.metastate != o.metastate) return std::less<const MetaState *>{}(m_key.metastate, o.metastate) ? -1 : 1;
        if (m_key.marking != o.marking) return std::less<const Marking *>{}(m_key.marking, o.marking) ? -1 : 1;
        return 0;
    }

    [[nodiscard]] const DSSStateKey &getKey() const {
        return m_key;
    }
};


#endif //DISTRIBUTEDSTATESPACE_DSSSTATE_H
