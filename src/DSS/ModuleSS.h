//
// Created by chiheb on 13/07/22.
//

#ifndef DISTRIBUTEDSTATESPACE_MODULESS_H
#define DISTRIBUTEDSTATESPACE_MODULESS_H

#include "DSS/ArcSync.h"
#include "DSS/MetaState.h"

class DistributedSS;

class ModuleSS {
public:
    ModuleSS(size_t id):m_id_module(id) {}

    MetaState *getInitialMS();

    bool insertMS(MetaState *ms);

    MetaState *findMetaStateByProductSCC(ProductSCC &productscc);

    friend ostream &operator<<(ostream &os, const ModuleSS &ss);

    size_t getMetaStateCount() const;

    MetaState *getMetaState(const int32_t &pos);

    vector<MetaState *> &getListMetaStates();

    void removeMetaState(MetaState *ms);

    void computeSyncClosedSCC(const MetaState *ms);

    size_t getIdModule() const { return m_id_module; }

    DistributedSS *getParent() const { return m_parent; }

    void setParent(DistributedSS *parent) { m_parent = parent; }
private:
    vector<MetaState *> m_metastates;
    size_t m_id_module;
    DistributedSS *m_parent {nullptr};
};


#endif //DISTRIBUTEDSTATESPACE_MODULESS_H
