// IntNode.h: interface for the IntNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTNODE_H__5E3DD10A_3004_475F_8BA4_15FFBBA28976__INCLUDED_)
#define AFX_INTNODE_H__5E3DD10A_3004_475F_8BA4_15FFBBA28976__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <iostream>
using namespace std;
class IntNode
{
public:
	virtual vector<IntNode> getNext();
	IntNode();
	virtual ~IntNode();

};

#endif // !defined(AFX_INTNODE_H__5E3DD10A_3004_475F_8BA4_15FFBBA28976__INCLUDED_)
