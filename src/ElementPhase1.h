// ElementPhase1.h: interface for the ElementPhase1 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEMENTPHASE1_H__3361E5F0_8986_4475_87F3_AC5D1C63A775__INCLUDED_)
#define AFX_ELEMENTPHASE1_H__3361E5F0_8986_4475_87F3_AC5D1C63A775__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Marking.h"
#include "ListMarquage.h"
class ElementPhase1
{
public:
	ElementPhase1(const ElementPhase1 &elt);
	bool etat;
	bool isExist(Marking * marq);
	ElementPhase1();
	virtual ~ElementPhase1();
	ListMarquage* groupe;
	ListMarquage liste_fils;
	ElementPhase1& operator =(const ElementPhase1 &elt);
};

#endif // !defined(AFX_ELEMENTPHASE1_H__3361E5F0_8986_4475_87F3_AC5D1C63A775__INCLUDED_)
