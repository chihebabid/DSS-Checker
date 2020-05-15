// PilePhase1.cpp: implementation of the PilePhase1 class.
//
//////////////////////////////////////////////////////////////////////


#include "PilePhase1.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PilePhase1::PilePhase1()
{
	m_liste.clear();

}

PilePhase1::~PilePhase1()
{

}
long PilePhase1::isCycle(Marking *marq)
{
	for (long i=0;i<m_liste.size();i++) {
		if (m_liste.at(i).groupe->existMarquage(marq)) return i;
	}
	return -1L;
}

