// Marking.cpp: implementation of the Marking class.
//
//////////////////////////////////////////////////////////////////////


#include "Marking.h"
#include "../Transition.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
Marking::Marking(const size_t size) : m_sccContainer(nullptr),m_data(size) {

}

Marking::~Marking() {

}


void Marking::add8BitsValue(const Octet val,const int index) {
    m_data[index]=val;
}


Octet Marking::get8BitsValue(const int index) {
    return m_data[index];
}

Marking::Marking(const Marking &value):m_data(value.m_data) {

}


Marking &Marking::operator=(const Marking &newvec) {
    if (this == &newvec) return *this;
    this->m_data = newvec.m_data;
    return *this;
}

void Marking::addSucc(Transition *t, Marking *m) {
    ml_succ.push_back(pair<Transition *, Marking *>(t, m));
}

vector<pair<Transition *, Marking *>> &Marking::getListSucc() {
    return ml_succ;
}

SCC *Marking::getSCCContainer() const {
    return m_sccContainer;
}

void Marking::setSCCContainer(SCC *c) {
    m_sccContainer=c;
}

ostream &operator<<(ostream &stream, const Marking &m) {
    for (auto elt: m.ml_succ) {
        Transition *t = elt.first;
        stream << t->getName();
        stream << "\n";
    }
    return stream;
}
