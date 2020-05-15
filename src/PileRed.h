// PileRed.h: interface for the PileRed class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PILERED_H__717B6507_BA66_42E4_907C_1FD81054155E__INCLUDED_)
#define AFX_PILERED_H__717B6507_BA66_42E4_907C_1FD81054155E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ElementRed.h"


class PileRed
{
public:
	int isCycle(Marking *marq);
	PileRed();
	virtual ~PileRed();
	vector<ElementRed> m_liste;
};

#endif // !defined(AFX_PILERED_H__717B6507_BA66_42E4_907C_1FD81054155E__INCLUDED_)
