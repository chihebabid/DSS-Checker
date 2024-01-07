//
// Created by chiheb on 04/10/23.
//

#include "DSSKripke.h"
#include "DSSState.h"

DSSKripke::DSSKripke(const spot::bdd_dict_ptr &dict_ptr,ModuleSS *metagraph): spot::kripke(dict_ptr),m_metagraph(metagraph)
{
    DSSIterator::m_dict_ptr=&dict_ptr;
}

spot::state* DSSKripke::get_init_state() const {
    return new DSSState(m_metagraph->getMetaState(0)->getInitialSCC());
}

DSSIterator* DSSKripke::succ_iter(const spot::state* s) const {
    auto ss = static_cast<const DSSState*>(s);
    auto scc=ss->getSCC();

    bdd cond = state_condition(ss);
    return new DSSIterator(scc,cond);

}
std::string DSSKripke::format_state(const spot::state* s) const {
    return "";
}
bdd DSSKripke::state_condition(const spot::state* s) const {
    return bddtrue;
}