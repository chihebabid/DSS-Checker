//
// Created by chiheb on 04/10/23.
//

#ifndef DISTRIBUTEDSTATESPACE_DSSITERATOR_H
#define DISTRIBUTEDSTATESPACE_DSSITERATOR_H
#include <spot/twa/twa.hh>
#include <vector>
#include "SpotMC/DSSState.h"

class DSSKripke;

// Successors of a marking in a DSSKripke
class DSSIterator : public spot::twa_succ_iterator {
public:
    DSSIterator(Marking *marking, const DSSKripke *kripke);
    bool first() override;
    bool next() override;
    bool done() const override;
    DSSState *dst() const override;
    bdd cond() const override;
    spot::acc_cond::mark_t acc() const override { return {}; }
private:
    const DSSKripke *m_kripke;
    uint32_t m_current_edge {};
    std::vector<std::pair<Marking *, Transition *>> m_succ; // (destination, fired transition or nullptr)
};


#endif //DISTRIBUTEDSTATESPACE_DSSITERATOR_H
