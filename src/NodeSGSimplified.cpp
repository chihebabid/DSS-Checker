// NodeSGSimplified.cpp: implementation of the NodeSGSimplified class.
//
//////////////////////////////////////////////////////////////////////


#include "NodeSGSimplified.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////







bool NodeSGSimplified::operator==(NodeSGSimplified &marq) {
    for (int i {}; i < marq.m_list.size(); ++i) {
        if (!(m_list[i] == marq.m_list[i])) return false;
    }
    return true;
}


void NodeSGSimplified::addMarquage(Marking &marquage) {
    m_list.push_back(marquage);
}

