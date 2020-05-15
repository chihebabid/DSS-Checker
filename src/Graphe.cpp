// Graphe.cpp: implementation of the Graphe class.
//
//////////////////////////////////////////////////////////////////////


#include "Graphe.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Graphe::Graphe()
{
	m_nodes= new vector<Marking>();
	m_arcs= new vector<Arc>();
	m_nodes->clear();
	m_arcs->clear();
}

Graphe::~Graphe()
{
	delete m_nodes;
	delete m_arcs;
}

vector<Marking>* Graphe::getListMarq() {
	return m_nodes;
}

void Graphe::addMarquage(Marking *m) {
	m_nodes->push_back(*m);
}

void Graphe::addArc(Arc *arc) {
	m_arcs->push_back(*arc);
}

bool Graphe::existMarquage(Marking* mar)  {
	return (m_nodes->end()!=find(m_nodes->begin(),m_nodes->end(),mar));
}
