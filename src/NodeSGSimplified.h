// NodeSGSimplified.h: interface for the NodeSGSimplified class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NODESGSIMPLIFIED_H__C0E45B46_BE36_4B2F_A970_B674E7C897CF__INCLUDED_)
#define AFX_NODESGSIMPLIFIED_H__C0E45B46_BE36_4B2F_A970_B674E7C897CF__INCLUDED_

#include "Marking.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class NodeSGSimplified
{
public:
	NodeSGSimplified(const NodeSGSimplified &noeud);
	void addMarquage(Marking& marquage);
	Marking* getMarquage(const int index) { return &m_list[index]; }
	void setVide(const int index);
	bool operator ==( NodeSGSimplified &marq);
	NodeSGSimplified& operator =(const NodeSGSimplified &val);
	NodeSGSimplified();
	virtual ~NodeSGSimplified();
	vector<Marking> m_list;
protected:

};


inline void NodeSGSimplified::setVide(const int index)
{
	m_list[index].setVide(true);
}


#endif // !defined(AFX_NODESGSIMPLIFIED_H__C0E45B46_BE36_4B2F_A970_B674E7C897CF__INCLUDED_)
