// NodeSGSimplified.cpp: implementation of the NodeSGSimplified class.
//
//////////////////////////////////////////////////////////////////////


#include "NodeSGSimplified.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NodeSGSimplified::NodeSGSimplified() {
    m_list.clear();
}

NodeSGSimplified::~NodeSGSimplified() {

}

NodeSGSimplified &NodeSGSimplified::operator=(const NodeSGSimplified &val) {
    m_list = val.m_list;
    return *this;
}

bool NodeSGSimplified::operator==(NodeSGSimplified &marq) {
    for (int i = 0; i < marq.m_list.size(); i++) {
        if (!(m_list.at(i) == marq.m_list.at(i))) return false;
    }
    return true;
}


void NodeSGSimplified::addMarquage(Marking &marquage) {
    m_list.push_back(marquage);
}

NodeSGSimplified::NodeSGSimplified(const NodeSGSimplified &noeud) {
    m_list = noeud.m_list;
}
