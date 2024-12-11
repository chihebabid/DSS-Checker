// Transition.cpp: implementation of the Transition class.
//
//////////////////////////////////////////////////////////////////////


#include "Transition.h"



bool Transition::isLocallyFirable() {
    for (auto & i_place : ml_input_places) {
        if (i_place.place->getTokens() < i_place.poids) return false;
    }
    return true;
}


void Transition::addPlaceEntree(Place *place, int poids) {
    ml_input_places.push_back({place,poids});


}

void Transition::addPlaceSortie(Place *place, int poids) {
    ml_output_places.push_back({place,poids});
}


void Transition::setSync(const bool enable) {
    m_sync = enable;
}

bool Transition::isFranchissable() {
    if (isSync()) return false;
    return isLocallyFirable();
}

////////////////////////////////////
// Fixer le code d'une transition //
////////////////////////////////////
void Transition::setCode(const int code) {
    m_code = code;
}

///////////////////////////////////////
// Renvoyer le code d'une transition //
///////////////////////////////////////
int Transition::getCode() {
    return m_code;
}

void Transition::setPetri(const int petri) {
    m_petri_num = petri;
}

int Transition::getPetri() {
    return m_petri_num;
}


