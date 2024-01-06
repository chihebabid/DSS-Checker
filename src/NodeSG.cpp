// NodeSG.cpp: implementation of the NodeSG class.
//
//////////////////////////////////////////////////////////////////////


#include "NodeSG.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NodeSG::NodeSG() {
    m_arcs = new vector<ArcSG *>();
}

NodeSG::~NodeSG() {
    delete m_arcs;
}


NodeSG &NodeSG::operator=(const NodeSG &val) {
    if (this == &val) return *this;
    m_list = val.m_list;
    m_equiv = val.m_equiv;
    return *this;
}

NodeSG::NodeSG(const NodeSG &val) {
    m_list = val.m_list;
    m_equiv = val.m_equiv;

}

long NodeSG::getCountArcs() {
    return m_arcs->size();
}





