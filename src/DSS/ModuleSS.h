//
// Created by chiheb on 13/07/22.
//

#ifndef DISTRIBUTEDSTATESPACE_MODULESS_H
#define DISTRIBUTEDSTATESPACE_MODULESS_H
#include "ArcSync.h"
#include "MetaState.h"

class ModuleSS {
public:
    MetaState* getInitialMS();
    bool insertMS(MetaState *ms);
    MetaState *findMetaStateByProductSCC(ProductSCC &productscc);
    friend ostream& operator<<(ostream& os, const ModuleSS& ss);
    size_t getMetaStateCount() const;
    MetaState* getMetaState(const int32_t & pos);
    vector<MetaState*> &getLMetaState();
    void removeMetaState(MetaState *ms);
private:
    vector<MetaState*> mlMetaState;
};



#endif //DISTRIBUTEDSTATESPACE_MODULESS_H
