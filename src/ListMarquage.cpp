// ListMarquage.cpp: implementation of the Marquage class.
//
//////////////////////////////////////////////////////////////////////


#include "ListMarquage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ListMarquage::ListMarquage()
{
	m_liste.clear();
}

ListMarquage::~ListMarquage()
{
	m_liste.clear();
}

bool ListMarquage::operator ==(ListMarquage &listemarquage)
{
	
	if (listemarquage.m_liste.size()==m_liste.size())  {
		
		for (int i=0;i<m_liste.size();i++) {
			if (!(existMarquage(&listemarquage.m_liste.at(i)))) return false;			
		}
		return true;
	}
	return false;
}	



ListMarquage& ListMarquage::operator =(const ListMarquage &elt)
{
	if (this==&elt) return *this;
	m_liste=elt.m_liste;
	return *this;
}




ListMarquage::ListMarquage(const ListMarquage &elt)
{
	m_liste=elt.m_liste;
}

void ListMarquage::addGroupe(ListMarquage& groupe)
{
	for (int i=0;i<groupe.getCount();i++)
		addMarquage(groupe.getMarquage(i));
}

Marking ListMarquage::pop()
{
	Marking marq=m_liste.back();
	m_liste.pop_back();
	return marq;
}
