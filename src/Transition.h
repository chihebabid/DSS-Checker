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
#include "PetriNet.h"    // Added by ClassView


class Transition : public Node {
public:
    inline int getInputPlacesCount() const {
        return ml_input_places.size();
    }

    int getPetri();

    void setPetri(const int petri);

    int getCode();

    void setCode(const int code);

    bool isFranchissable();

    void setSync(const bool enable);

    inline bool isSync() const {
        return m_sync;
    }

    void addPlaceSortie(Place *place, int poids);

    void addPlaceEntree(Place *place, int poids);

    inline void fire() {
        // Add tokens to output places
        size_t index {};
        for (auto & o_place : ml_output_places) {
            o_place->addTokens(m_poids_sorties[index]);
            ++index;
        }
        // Sub tokens from input places
        index=0;
        for (auto & in_place : ml_input_places) {
            in_place->subTokens(m_poids_entrees[index]);
            ++index;
        }
    }

    bool isLocallyFirable();

    Transition()=default;

    virtual ~Transition()=default;

private:
    int m_petri_num;
    int m_code;
    bool m_sync {false};
    vector<Place *> ml_input_places;
    vector<Place *> ml_output_places;
    vector<int> m_poids_entrees;
    vector<int> m_poids_sorties;
};

#endif // !defined(AFX_TRANSITION_H__AD708264_D0C2_490C_967C_95F9B5F7195D__INCLUDED_)
