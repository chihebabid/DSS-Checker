// PilePhase1.h: interface for the PilePhase1 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PILEPHASE1_H__A6A77ECD_3D0B_429D_BC00_AFF95EF6EECD__INCLUDED_)
#define AFX_PILEPHASE1_H__A6A77ECD_3D0B_429D_BC00_AFF95EF6EECD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ElementPhase1.h"
using std::vector;
class PilePhase1
{
public:
	long isCycle(Marking *marq);
	PilePhase1();
	virtual ~PilePhase1();
	vector<ElementPhase1> m_liste;

};

#endif // !defined(AFX_PILEPHASE1_H__A6A77ECD_3D0B_429D_BC00_AFF95EF6EECD__INCLUDED_)
