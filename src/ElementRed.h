// ElementRed.h: interface for the ElementRed class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ELEMENTRED_H__19D9D8D8_D797_4058_820B_43E8E344F5F4__INCLUDED_)
#define AFX_ELEMENTRED_H__19D9D8D8_D797_4058_820B_43E8E344F5F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
#include "Fils.h"
#include "ListMarquage.h"
using namespace std;
class ElementRed
{
public:

	void addFils(vector<Fils>& liste_fils);
	ElementRed(const ElementRed &elt);
	ElementRed& operator =(const ElementRed &elt);
	bool isExist(Marking * marq);
	ListMarquage* groupe;
	vector<Fils> liste_fils;
	bool etat;
	ElementRed();
	virtual ~ElementRed();
private :
	int test;
};

#endif // !defined(AFX_ELEMENTRED_H__19D9D8D8_D797_4058_820B_43E8E344F5F4__INCLUDED_)
