/*
 * MetaState.cpp
 *
 *  Created on: 17 sept. 2015
 *      Author: LIP2
 */

#include "MetaState.h"

MetaState::MetaState() {
	m_graph=NULL;
	m_id=0;
}

MetaState::~MetaState() {
	// TODO Auto-generated destructor stub
}

void MetaState::setStateGraph(StateGraph* aut){

	m_graph=aut;
}

StateGraph* MetaState::getStateGraph() {
    return m_graph;
}

void MetaState::setId(unsigned int id) {
	m_id=id;
}

vector<Marking*>*  MetaState::getListMarq() {
	return m_graph->getListMarquages();
}

/*vector<InternalArc>* MetaState::getListArcs() {
    return m_graph->getListArcs();
}*/

vector<SCC*>* MetaState::getListSCCs() {
    return m_graph->getListSCCs();
}

SCC* MetaState::findSCC(Marking *state) {
    return m_graph->findSCC(state);
}

SCC* MetaState::getInitialSCC() {
    return m_graph->findSCC(m_graph->getListMarquages()->at(0));
}


ProductSCC * MetaState::getSCCProductName() {
    return m_name;
}

void MetaState::setSCCProductName(ProductSCC* name) {
    m_name=name;
}


void MetaState::addSyncArc(ArcSync *sync_arc) {
    m_arcs.push_back(sync_arc);
}

vector<ArcSync*> MetaState::getSucc() {
    return m_arcs;
}
uint32_t MetaState::getArcCount() {
    m_graph->getCountArcs();
}

bool MetaState::operator==(const MetaState &ms) {
    return true;
}
