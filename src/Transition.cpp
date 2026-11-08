// Transition.cpp: implementation of the Transition class.
//
//////////////////////////////////////////////////////////////////////


#include "Transition.h"



bool Transition::isLocallyFirable() {
    for (auto & i_place : m_input_places) {
        if (i_place.place->getTokens() < i_place.weight) {
            return false
        }
    }
    return true;
}


void Transition::addInputPlace(Place *place, int weight) {
    m_input_places.push_back({place,weight});


}

void Transition::addOutputPlace(Place *place, int weight) {
    m_output_places.push_back({place,weight});
}


void Transition::setSync(const bool enable) {
    m_sync = enable;
}

bool Transition::isFirable() {
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


