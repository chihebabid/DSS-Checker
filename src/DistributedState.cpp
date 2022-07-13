/*
 * DistributedState.cpp
 *
 *  Created on: 17 sept. 2015
 *      Author: LIP2
 */

#include "DistributedState.h"

DistributedState::DistributedState(ModularPetriNet *petri)
{
	m_petri=petri;

}

DistributedState::~DistributedState() {
	
}



void DistributedState:: build(){
	cout<<"Building Distributed state space..."<<endl;

}


MetaGraph* DistributedState:: getMetaGraph(const int index){
	return ml_metagraph.at(index);
}

void DistributedState::setNombreModules(const int n)
{
	ml_metagraph.resize(n);
	for (int i=0;i<n;i++)
		ml_metagraph[i]=new MetaGraph();

}

void DistributedState::addMetaState(MetaState* ms, int module)
{
	ml_metagraph[module]->addMetaState(ms);
}

vector<Marking*>* DistributedState::getLocalStates(ProductSCC* productscc,const int module) {
    return ml_metagraph[module]->getListLocalStates(productscc);
}
