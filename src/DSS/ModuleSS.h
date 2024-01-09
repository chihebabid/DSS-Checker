//
// Created by chiheb on 13/07/22.
//

#ifndef DISTRIBUTEDSTATESPACE_MODULESS_H
#define DISTRIBUTEDSTATESPACE_MODULESS_H

#include "../ArcSync.h"
#include "MetaState.h"

class ModuleSS {
public:
    ModuleSS(size_t id):m_id_module(id) {}

    MetaState *getInitialMS();

    bool insertMS(MetaState *ms);

    MetaState *findMetaStateByProductSCC(ProductSCC &productscc);

    friend ostream &operator<<(ostream &os, const ModuleSS &ss);

    size_t getMetaStateCount() const;

    MetaState *getMetaState(const int32_t &pos);

    vector<MetaState *> &getLMetaState();

    void removeMetaState(MetaState *ms);

    size_t getIDModule() const { return m_id_module; }
private:
    vector<MetaState *> mlMetaState;
    size_t m_id_module;
};


#endif //DISTRIBUTEDSTATESPACE_MODULESS_H
