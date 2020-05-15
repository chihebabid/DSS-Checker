// Arc.cpp: implementation of the Arc class.
//
//////////////////////////////////////////////////////////////////////


#include "Arc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Arc::Arc()
{

}

Arc::~Arc()
{

}

void Arc::setSource(ListMarquage *marq)
{
	m_source=marq;
}

void Arc::setDestination(ListMarquage *marq)
{
	m_destination=marq;
}

ListMarquage* Arc::getSource()
{
	return m_source;
}

ListMarquage* Arc::getDestination()
{
	return m_destination;
}

bool Arc::operator ==(const Arc &val)
{
	return (*m_source==*val.m_source) && (*m_destination==*val.m_destination) && (m_name==val.m_name);
}
