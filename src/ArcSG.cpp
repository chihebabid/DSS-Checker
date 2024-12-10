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

NodeSGSimplified *ArcSG::getMarquageSource() {
    return &m_marquage_depart;
}


bool ArcSG::operator==(ArcSG &arc) {
    return ((m_destination == arc.m_destination) && (m_marquage_depart == arc.m_marquage_depart) && (m_fusion == arc.m_fusion));
}



