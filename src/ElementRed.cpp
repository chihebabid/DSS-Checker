// ElementRed.cpp: implementation of the ElementRed class.
//
//////////////////////////////////////////////////////////////////////


#include "ElementRed.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ElementRed::ElementRed()
{
}

ElementRed::~ElementRed()
{

}

bool ElementRed::isExist(Marking *marq)
{
	return groupe->existMarquage(marq);
}

ElementRed& ElementRed::operator =(const ElementRed &elt)
{
	if (this==&elt) return *this;
	this->groupe=elt.groupe;
	this->etat=elt.etat;	
	this->liste_fils=elt.liste_fils;
	return *this;
}

ElementRed::ElementRed(const ElementRed &elt)
{
	this->groupe=elt.groupe;
	this->etat=elt.etat;
	this->liste_fils=elt.liste_fils;
}

void ElementRed::addFils(vector<Fils>& liste)
{
	for (int i=0;liste_fils.size();i++) 
	{
		if (!isExist(&liste.at(i).getMarquage())) 
			liste_fils.push_back(liste.at(i));
	}
}
