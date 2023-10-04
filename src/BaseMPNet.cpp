//
// Created by chiheb on 13/07/22.
//

#include "BaseMPNet.h"


PetriNet *BaseMPNet::getModule(const int index) {
    return m_modules[index];
}

BaseMPNet::~BaseMPNet() {
    for (const auto &elt: m_modules)
        delete elt;
}