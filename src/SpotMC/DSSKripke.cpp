//
// Created by chiheb on 04/10/23.
//

#include "DSSKripke.h"
#include "DSSState.h"

DSSKripke::DSSKripke(const spot::bdd_dict_ptr &dict_ptr,MetaGraph *metagraph): spot::kripke(dict_ptr),m_metagraph(metagraph)
{

    // DSSKripkeIteratorv2::m_dict_ptr=&dict_ptr;
}

spot::state* DSSKripke::get_init_state() const {
    //cout<<"Initial state given...\n";
    return new DSSState(m_metagraph->getMetaState(0)->getInitialSCC());
}