/*
 * DistributedState.h
 *
 *  Created on: 17 sept. 2015
 *      Author: LIP2
 */

#ifndef DISTRIBUTEDSTATE_H_
#define DISTRIBUTEDSTATE_H_

#include "DSS/MetaState.h"
#include "DSS/MetaGraph.h"

class ModularPetriNet;

class DistributedState {
public:
    MetaGraph *getMetaGraph(const int index);

    DistributedState(ModularPetriNet *petri);

    void setNombreModules(const int n);

    void addMetaState(MetaState *ms, int module);

    virtual ~DistributedState();

    vector<Marking *> *getLocalStates(ProductSCC *productscc, const int module);

private:
    ModularPetriNet *m_petri;
    vector<MetaGraph *> ml_metagraph;

};

#endif /* DISTRIBUTEDSTATE_H_ */
