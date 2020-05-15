// ArcSG.cpp: implementation of the ArcSG class.
//
//////////////////////////////////////////////////////////////////////


#include "ArcSG.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ArcSG::ArcSG()
{

}

ArcSG::~ArcSG()
{

}



NodeSG* ArcSG::getNodeSGDest()
{
	return m_destination;
}

NodeSGSimplified* ArcSG::getMarquageSource()
{
	return &m_marquage_depart;
}



bool ArcSG::operator ==(ArcSG &arc)
{
	return ((m_destination==arc.m_destination) && (m_marquage_depart==arc.m_marquage_depart) && (m_fusion==arc.m_fusion) );
}


ArcSG& ArcSG::operator =(const ArcSG &val)
{
	m_destination=val.m_destination;
	m_marquage_depart=val.m_marquage_depart;
	m_fusion=val.m_fusion;
	return *this;
}

ArcSG::ArcSG(const ArcSG &val)
{
	m_destination=val.m_destination;
	m_marquage_depart=val.m_marquage_depart;
	m_fusion=val.m_fusion;

}



void ArcSG::setDestination(NodeSG *destination)
{
	m_destination=destination;
}
