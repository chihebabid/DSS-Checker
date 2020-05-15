// Arc.h: interface for the Arc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARC_H__F0C2D1F9_4B49_4827_949E_0E3D52DD796E__INCLUDED_)
#define AFX_ARC_H__F0C2D1F9_4B49_4827_949E_0E3D52DD796E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ListMarquage.h"
#include "Node.h"
class Arc : public Node
{
public:
	bool operator ==(const Arc &val);
	ListMarquage* getDestination();
	ListMarquage* getSource();
	void setDestination(ListMarquage* marq);
	void setSource(ListMarquage* marq);
	Arc();
	virtual ~Arc();

private:
	ListMarquage* m_destination;
	ListMarquage* m_source;
};

#endif // !defined(AFX_ARC_H__F0C2D1F9_4B49_4827_949E_0E3D52DD796E__INCLUDED_)
