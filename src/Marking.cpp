// Marking.cpp: implementation of the Marking class.
//
//////////////////////////////////////////////////////////////////////


#include "Marking.h"
#include "Transition.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Marking::Marking() : m_vide(false), m_sccContainer(nullptr) {

}

Marking::~Marking() {

}


void Marking::add8BitsValue(const Octet val) {
    m_vector.push_back(val);
}


Octet Marking::get8BitsValue(const int index) {
    return m_vector[index].to_ulong();
}

Marking::Marking(const Marking &value) {

    m_vide = value.m_vide;
    m_vector = value.m_vector;

}


Marking &Marking::operator=(const Marking &newvec) {
    if (this == &newvec) return *this;
    this->m_vide = newvec.m_vide;
    this->m_vector = newvec.m_vector;
    return *this;
}

void Marking::addSucc(Transition *t, Marking *m) {
    ml_succ.push_back(pair<Transition *, Marking *>(t, m));
}

vector<pair<Transition *, Marking *>> *Marking::getListSucc() {
    return &ml_succ;
}

SCC *Marking::getSccContainer() const {
    return m_sccContainer;
}

ostream &operator<<(ostream &stream, const Marking &m) {
    for (auto elt: m.ml_succ) {
        Transition *t = elt.first;
        stream << t->getName();
        stream << "\n";
    }
    return stream;
}
