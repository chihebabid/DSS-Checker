// ArcSG.cpp: implementation of the ArcSG class.
//
//////////////////////////////////////////////////////////////////////


#include "ArcSG.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////




NodeSG *ArcSG::getNodeSGDest() {
    return m_destination;
}

NodeSGSimplified *ArcSG::getSourceMarking() {
    return &m_source_marking;
}


bool ArcSG::operator==(ArcSG &arc) {
    return ((m_destination == arc.m_destination) && (m_source_marking == arc.m_source_marking) && (m_fusion == arc.m_fusion));
}



