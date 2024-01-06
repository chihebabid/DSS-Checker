//
// Created by chiheb on 04/10/23.
//

#ifndef DISTRIBUTEDSTATESPACE_DSSKRIPKE_H
#define DISTRIBUTEDSTATESPACE_DSSKRIPKE_H
#include <spot/kripke/kripke.hh>
#include "DSS/ModuleSS.h"
#include "DSSIterator.h"
class DSSKripke : public spot::kripke {
    ModuleSS * m_metagraph;
public:
    DSSKripke ( const spot::bdd_dict_ptr& dict_ptr, ModuleSS *metagraph);
    spot::state* get_init_state() const override;
    DSSIterator* succ_iter(const spot::state* s) const override;
    std::string format_state(const spot::state* s) const override;
    bdd state_condition(const spot::state* s) const override;

};


#endif //DISTRIBUTEDSTATESPACE_DSSKRIPKE_H
