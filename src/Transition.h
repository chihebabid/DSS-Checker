// Transition.h: interface for the Transition class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRANSITION_H__AD708264_D0C2_490C_967C_95F9B5F7195D__INCLUDED_)
#define AFX_TRANSITION_H__AD708264_D0C2_490C_967C_95F9B5F7195D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Node.h"
#include "Place.h"
#include "PetriNet.h"	// Added by ClassView


class Transition : public Node
{
public:
	inline int getPlacesEntreesCount(){
	return m_places_entrees.size();
}

	int getPetri();
	void setPetri(const int petri);
	int getCode();
	void setCode(const int code);
	bool isFranchissable();
	void setSync(const bool enable);
	inline bool isSync()
	{
		return m_sync;
	}
	void addPlaceSortie(Place* place,int poids);
	void addPlaceEntree(Place* place,int poids);
	inline void tirer()
	{
		// Ajouter les jetons aux places de sorties
		for (unsigned int i=0;i<m_places_sorties.size();i++) {
			m_places_sorties.at(i)->addTokens(m_poids_sorties.at(i));
		}
		//Supprimer les jetons � partir de places d'entr�es
		for (unsigned int i=0;i<m_places_entrees.size();i++) {
			m_places_entrees.at(i)->subTokens(m_poids_entrees.at(i));

		}
	}
	bool isFranchissableLocal();
	Transition();
	virtual ~Transition();
private:
	int m_petri_num;
	int m_code;
	bool m_sync;
	vector<Place*> m_places_entrees;
	vector<Place*> m_places_sorties;
	vector<int> m_poids_entrees;
	vector<int> m_poids_sorties;


};

#endif // !defined(AFX_TRANSITION_H__AD708264_D0C2_490C_967C_95F9B5F7195D__INCLUDED_)
