// Transition.cpp: implementation of the Transition class.
//
//////////////////////////////////////////////////////////////////////


#include "Transition.h"



bool Transition::isFranchissableLocal() {
    for (int i = 0; i < ml_input_places.size(); i++) {
        if (ml_input_places[i]->getTokens() < m_poids_entrees.at(i)) return false;

    }
    return true;
}


void Transition::addPlaceEntree(Place *place, int poids) {
    ml_input_places.push_back(place);
    m_poids_entrees.push_back(poids);

}

void Transition::addPlaceSortie(Place *place, int poids) {
    ml_output_places.push_back(place);
    m_poids_sorties.push_back(poids);

}


void Transition::setSync(const bool enable) {
    m_sync = enable;
}

bool Transition::isFranchissable() {
    if (isSync()) return false;
    return isFranchissableLocal();
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


