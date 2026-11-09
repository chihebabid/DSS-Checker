// Marking.cpp: implementation of the Marking class.
//
//////////////////////////////////////////////////////////////////////


#include "DSS/Marking.h"
#include "Petri/Transition.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
Marking::Marking(const size_t size) : m_scc_container(nullptr),m_data(size) {

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
    m_succ.push_back(pair<Transition *, Marking *>(t, m));
}

vector<pair<Transition *, Marking *>> &Marking::getListSucc() {
    return m_succ;
}

SCC *Marking::getSCCContainer() const {
    return m_scc_container;
}

void Marking::setSCCContainer(SCC *c) {
    m_scc_container=c;
}

ostream &operator<<(ostream &stream, const Marking &m) {
    for (auto elt: m.m_succ) {
        Transition *t = elt.first;
        stream << t->getName();
        stream << "\n";
    }
    return stream;
}
