// Fils.cpp: implementation of the Fils class.
//
//////////////////////////////////////////////////////////////////////


#include "Fils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Fils::Fils() {

}

Fils::~Fils() {

}

bool Fils::operator==(Fils &val) {
    for (int i = 0; i < val.m_list_transitions.size(); i++)
        if (!isTransitionExist(val.m_list_transitions.at(i))) return false;

    return (this->m_marq == val.m_marq);
}

//////////////////////////////////////////////////////////////////////
// V�rifier si une transition existe dans la liste
//////////////////////////////////////////////////////////////////////
bool Fils::isTransitionExist(const Transition &t) {
    bool result = false;
    for (int i = 0; i < m_list_transitions.size() && !result; i++)
        if (t.getName() == m_list_transitions.at(i).getName()) result = true;
    return result;
}

Fils &Fils::operator=(const Fils &fils) {
    if (this == &fils) return *this;
    m_list_transitions = fils.m_list_transitions;
    m_marq = fils.m_marq;

    return *this;
}

void Fils::addTransition(const Transition &t) {
    if (!isTransitionExist(t)) m_list_transitions.push_back(t);
}

Marking &Fils::getMarquage() {

    return m_marq;
}

void Fils::setMarquage(Marking &marq) {
    m_marq = marq;

}

////////////////////////////////////////////////////////////////
// Retourner la liste des transitions associ�es � ce marquage
////////////////////////////////////////////////////////////////
vector<Transition> &Fils::getListeTransitions() {
    return m_list_transitions;
}


Fils::Fils(const Fils &val) {
    m_list_transitions = val.m_list_transitions;
    m_marq = val.m_marq;

}
