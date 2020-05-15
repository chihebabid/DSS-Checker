// Fils.h: interface for the Fils class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILS_H__9BD001F3_1C4F_48A0_8CF9_FE36941F5D02__INCLUDED_)
#define AFX_FILS_H__9BD001F3_1C4F_48A0_8CF9_FE36941F5D02__INCLUDED_

#include "Marking.h"	// Added by ClassView
#include "Transition.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Transition;
class Fils
{
public:
	Fils(const Fils &val);
	vector<Transition>& getListeTransitions();
	void setMarquage(Marking& marq);
	Marking& getMarquage();
	void addTransition(const Transition& t);
	Fils();
	virtual ~Fils();
	bool operator ==(Fils &val);
	Fils& operator =(const Fils &fils);
private:
	bool isTransitionExist(const Transition& t);
	vector<Transition> m_list_transitions;
	Marking m_marq;
};

#endif // !defined(AFX_FILS_H__9BD001F3_1C4F_48A0_8CF9_FE36941F5D02__INCLUDED_)
