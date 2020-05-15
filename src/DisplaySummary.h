// DisplaySummary.h: interface for the DisplaySummary class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISPLAYSUMMARY_H__340BD355_4A6C_4F8B_8273_CDDE34FB0EA0__INCLUDED_)
#define AFX_DISPLAYSUMMARY_H__340BD355_4A6C_4F8B_8273_CDDE34FB0EA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ModularPetriNet.h"
class DisplaySummary
{
public:
	void display();

	void setPetr(CModularPetriNet* petri);
	DisplaySummary();
	virtual ~DisplaySummary();
private :
	CModularPetriNet* m_petri;

};

#endif // !defined(AFX_DISPLAYSUMMARY_H__340BD355_4A6C_4F8B_8273_CDDE34FB0EA0__INCLUDED_)
