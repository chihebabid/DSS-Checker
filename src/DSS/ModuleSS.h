//
// Created by chiheb on 13/07/22.
//

#ifndef DISTRIBUTEDSTATESPACE_MODULESS_H
#define DISTRIBUTEDSTATESPACE_MODULESS_H
#include "MetaState.h"

class ModuleSS {
public:
    MetaState* getInitialMS();
    bool insertMS(MetaState *ms);
    MetaState *findMetaStateByProductSCC(ProductSCC &productscc);
private:
    vector<MetaState*> mlMetaState;
};


#endif //DISTRIBUTEDSTATESPACE_MODULESS_H
