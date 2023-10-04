// ElementPhase1.cpp: implementation of the ElementPhase1 class.
//
//////////////////////////////////////////////////////////////////////


#include "ElementPhase1.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ElementPhase1::ElementPhase1() {
    etat = false;
}

ElementPhase1::~ElementPhase1() {

}

bool ElementPhase1::isExist(Marking *marq) {
    return groupe->existMarquage(marq);
}

ElementPhase1 &ElementPhase1::operator=(const ElementPhase1 &elt) {
    this->groupe = elt.groupe;
    this->liste_fils = elt.liste_fils;
    this->etat = elt.etat;
    return *this;
}

ElementPhase1::ElementPhase1(const ElementPhase1 &elt) {
    this->groupe = elt.groupe;
    this->etat = elt.etat;
    this->liste_fils = elt.liste_fils;
}
