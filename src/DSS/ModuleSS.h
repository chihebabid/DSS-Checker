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
private:
    vector<MetaState*> mlMS;
};


#endif //DISTRIBUTEDSTATESPACE_MODULESS_H
