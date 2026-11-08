// NodeSGSimplified.cpp: implementation of the NodeSGSimplified class.
//
//////////////////////////////////////////////////////////////////////


#include "NodeSGSimplified.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////







bool NodeSGSimplified::operator==(NodeSGSimplified &other) {
    for (int i {}; i < other.m_list.size(); ++i) {
        if (!(m_list[i] == other.m_list[i])) return false;
    }
    return true;
}


void NodeSGSimplified::addMarking(Marking &marking) {
    m_list.push_back(marking);
}

