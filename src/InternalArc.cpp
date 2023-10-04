
#include "InternalArc.h"

InternalArc::InternalArc() {
    m_source = NULL;
    m_destination = NULL;
    m_transition = NULL;
}

InternalArc::~InternalArc() {
    //dtor
}

void InternalArc::setSource(Marking *source) {
    m_source = source;
}

void InternalArc::setDestination(Marking *dest) {
    m_destination = dest;
}

void InternalArc::setTranition(Transition *tr) {
    m_transition = tr;
}

Marking *InternalArc::getSource() {
    return m_source;
}

Marking *InternalArc::getDestination() {
    return m_destination;
}


Transition *InternalArc::getTransition() {
    return m_transition;
}
