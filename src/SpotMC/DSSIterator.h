//
// Created by chiheb on 04/10/23.
//

#ifndef DISTRIBUTEDSTATESPACE_DSSITERATOR_H
#define DISTRIBUTEDSTATESPACE_DSSITERATOR_H
#include <spot/kripke/kripke.hh>
#include <vector>
#include "DSSState.h"
class DSSIterator : public spot::kripke_succ_iterator {
    uint32_t m_edge_count;
    uint32_t m_current_edge;
    std::vector<std::tuple<SCC*,MetaState*,int>> m_lsucc;
public:
    static spot::bdd_dict_ptr* m_dict_ptr;
    bool first() override;
    bool next() override;
    bool done() const override;
    DSSState* dst() const override;
    bdd cond() const override final;
};


#endif //DISTRIBUTEDSTATESPACE_DSSITERATOR_H
