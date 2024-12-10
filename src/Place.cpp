// Place.cpp: implementation of the Place class.
//
//////////////////////////////////////////////////////////////////////


#include "Place.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


uint32_t Place::getTokens() const {
    return m_nb_tokens;
}

void Place::setTokens(const uint32_t value) {
    m_nb_tokens = value;
}

void Place::addTokens(const uint32_t addValue) {
    m_nb_tokens += addValue;
}

void Place::subTokens(const uint32_t nb) {
    m_nb_tokens -= nb;
}

