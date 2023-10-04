//
// Created by chiheb on 04/10/23.
//

#ifndef DISTRIBUTEDSTATESPACE_DSSKRIPKE_H
#define DISTRIBUTEDSTATESPACE_DSSKRIPKE_H
#include <spot/kripke/kripke.hh>
#include "../MetaGraph.h"

class DSSKripke : public spot::kripke {
    MetaGraph* m_metagraph;
public:
    DSSKripke ( const spot::bdd_dict_ptr& dict_ptr, MetaGraph *metagraph);
    spot::state* get_init_state() const;
};


#endif //DISTRIBUTEDSTATESPACE_DSSKRIPKE_H
