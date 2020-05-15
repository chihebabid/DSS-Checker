// ListSync.h: interface for the ListSync class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTSYNC_H__E0EEADD3_02F7_475B_B3FA_73A1CF0657BA__INCLUDED_)
#define AFX_LISTSYNC_H__E0EEADD3_02F7_475B_B3FA_73A1CF0657BA__INCLUDED_

#include "Marking.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Transition.h"
class ListSync
{
public:
	Marking getMarquage(const int index);
	ListSync & operator =(const ListSync &list);
	int getCount();
	void empty();
	void addTransition(Transition t,Marking marq);
	ListSync();
	virtual ~ListSync();
private:
	vector<Transition> m_list_sync;
	vector<Marking> m_list_marq;

};

#endif // !defined(AFX_LISTSYNC_H__E0EEADD3_02F7_475B_B3FA_73A1CF0657BA__INCLUDED_)
