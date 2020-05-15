// Fusion.cpp: implementation of the Fusion class.
//
//////////////////////////////////////////////////////////////////////

#include "Transition.h"
#include "Fusion.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Fusion::Fusion()
{

}

Fusion::~Fusion()
{

}

void Fusion::tirer() {
    for (int i=0;i<m_transitions.size();i++) m_transitions[i]->tirer();
}


void Fusion::addTransition(Transition *transition)
{
	m_transitions.push_back(transition);
}

bool Fusion::isFranchissable()
{
	bool franchissable=true;
	for (int i=0;i<m_transitions.size() && franchissable;i++)
		franchissable=m_transitions[i]->isFranchissableLocal();
	return franchissable;
}

string Fusion::getFusionName()
{
	return m_transitions[0]->getName();
}

int Fusion::getCountTransitionsLocales()
{
	return m_transitions.size();
}


/////////////////////////////////////////////////////////////////////
// Indiquer si le module 'index_module' participe � la communication
///////////////////////////////////////////////////////////////////////
bool Fusion::participatePartially(const int index_module)
{
 	for (int i=0;i<m_transitions.size();i++)		{
 		if (m_transitions[i]->getPetri()==index_module)
			if (m_transitions[i]->getPlacesEntreesCount()>0) {
 			return true;
 		}
 	}
 	return false;
}
/////////////////////////////////////////////////////////////////////
// Indiquer si le module 'index_module' participe � la communication
///////////////////////////////////////////////////////////////////////
bool Fusion::participate(const int index_module)
{

	for (int i=0;i<m_transitions.size();i++)
	{
		if (m_transitions[i]->getPetri()==index_module)
		{
			return true;
		}
	}
	return false;
}




bool Fusion::isFranchissableLocal(int module)
{
	bool result=false;
	for (int i=0;i<m_transitions.size() && !result;i++)		{
		if (m_transitions[i]->getPetri()==module)
		{
			return m_transitions.at(i)->isFranchissableLocal();
		}
	}
	return false;
}
