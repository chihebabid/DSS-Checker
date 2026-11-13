#include "DSS/SCC.h"

bool SCC::isEqual(const SCC &scc) const {
    if (scc.m_list.size() != m_list.size()) return false;
    if (m_list.empty()) return true;
    if (this == &scc) return true;
    // The marking of this metastate equal to the first marking of scc must belong to this SCC
    // (hash lookup in the metastate instead of a linear search in the SCC)
    const Marking *same {getParent()->existMarking(scc.m_list[0])};
    return same && same->getSCCContainer() == this;
}

size_t SCC::contentHash() const {
    if (!m_hash_valid) {
        // Sum of mixed hashes: independent of the order of the markings
        size_t hash {m_list.size()};
        for (const auto marking: m_list) {
            size_t h {marking->hash()};
            h ^= h >> 33;
            h *= 0xff51afd7ed558ccdULL;
            h ^= h >> 33;
            hash += h;
        }
        m_hash = hash;
        m_hash_valid = true;
    }
    return m_hash;
}

size_t SCC::getCount() const {
    return m_list.size();
}

void SCC::addState(Marking *m) {
    m->setSCCContainer(this);
    m_list.emplace_back(m);
    m_hash_valid = false;
}

vector<Marking *> *SCC::getListStates() {
    return &m_list;
}
