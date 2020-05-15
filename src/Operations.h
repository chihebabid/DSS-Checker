// Operations.h: interface for the Operations class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPERATIONS_H__30549F09_4977_4332_8F36_71DFCA0C048A__INCLUDED_)
#define AFX_OPERATIONS_H__30549F09_4977_4332_8F36_71DFCA0C048A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
#include "ArcSG.h"
using namespace std;
class Operations
{
public:
	static bool findArc(vector<ArcSG*>* liste,ArcSG* arc);
	static void reduceArcs(vector<ArcSG*>** list_arcs);
	static bool verifier(vector<Transition*>& liste,string transition);
	static bool find(vector<string> liste,string value);
	static bool find(vector<Marking> list,Marking  *marq);
	Operations();
	virtual ~Operations();

};

#endif // !defined(AFX_OPERATIONS_H__30549F09_4977_4332_8F36_71DFCA0C048A__INCLUDED_)
