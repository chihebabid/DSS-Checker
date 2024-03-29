//
// Created by chiheb on 04/10/23.
//

#ifndef DISTRIBUTEDSTATESPACE_DSSITERATOR_H
#define DISTRIBUTEDSTATESPACE_DSSITERATOR_H
#include <spot/kripke/kripke.hh>
#include <vector>
#include "DSSState.h"
class DSSIterator : public spot::kripke_succ_iterator {
    SCC *m_scc;

    uint32_t m_current_edge;
    std::vector<std::pair<SCC*,Transition*>> m_lsucc;
public:
    static spot::bdd_dict_ptr* m_dict_ptr;
    DSSIterator ( SCC* scc, bdd cnd );
    bool first() override;
    bool next() override;
    bool done() const override;
    DSSState* dst() const override;
    bdd cond() const override final;
};


#endif //DISTRIBUTEDSTATESPACE_DSSITERATOR_H
