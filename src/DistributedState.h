/*
 * DistributedState.h
 *
 *  Created on: 17 sept. 2015
 *      Author: LIP2
 */

#ifndef DISTRIBUTEDSTATE_H_
#define DISTRIBUTEDSTATE_H_
#include "MetaState.h"
#include "MetaGraph.h"
class CModularPetriNet;
class DistributedState {
    public:
        void build();
        MetaGraph* getMetaGraph ( const int index );
        DistributedState ( CModularPetriNet* petri );
        void setNombreModules ( const int n );
        void addMetaState ( MetaState* ms, int module );
        virtual ~DistributedState();
        vector<Marking*>* getLocalStates ( ProductSCC* productscc,const int module );

    private:
        CModularPetriNet* m_petri;
        vector<MetaGraph*> ml_metagraph;

    };

#endif /* DISTRIBUTEDSTATE_H_ */
