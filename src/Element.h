// Element.h: interface for the Element class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEMENT_H__4339919C_C19F_410C_8EEE_CB59B2493083__INCLUDED_)
#define AFX_ELEMENT_H__4339919C_C19F_410C_8EEE_CB59B2493083__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Marking.h"
#include "Transition.h"
class Element
{
public:
	Element(const Element &elt);
	Marking marquage;
	vector<Transition*> liste_transitions;
	Element();
	virtual ~Element();

};

#endif // !defined(AFX_ELEMENT_H__4339919C_C19F_410C_8EEE_CB59B2493083__INCLUDED_)
