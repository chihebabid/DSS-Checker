//
// Created by chiheb on 04/10/23.
//

#ifndef DISTRIBUTEDSTATESPACE_DSSITERATOR_H
#define DISTRIBUTEDSTATESPACE_DSSITERATOR_H
#include <spot/twa/twa.hh>
#include <vector>
#include "SpotMC/DSSState.h"

class DSSKripke;

// Successors of a state in a DSSKripke
class DSSIterator : public spot::twa_succ_iterator {
public:
    DSSIterator(const DSSStateKey &state, const DSSKripke *kripke);
    bool first() override;
    bool next() override;
    bool done() const override;
    DSSState *dst() const override;
    bdd cond() const override;
    spot::acc_cond::mark_t acc() const override { return {}; }
private:
    const DSSKripke *m_kripke;
    uint32_t m_current_edge {};
    const std::vector<std::pair<DSSStateKey, Transition *>> &m_succ; // (destination, fired transition or nullptr), cached by the DSSKripke
};


#endif //DISTRIBUTEDSTATESPACE_DSSITERATOR_H
