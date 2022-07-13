//
// Created by chiheb on 13/07/22.
//

#include "BaseBuilder.h"


PetriNet* BaseBuilder::getModule(const int index) {
    return m_modules[index];
}

BaseBuilder::~BaseBuilder() {
    for (const auto & elt : m_modules)
        delete elt;
}