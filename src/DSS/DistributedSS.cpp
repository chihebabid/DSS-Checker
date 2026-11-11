//
// Created by chiheb on 11/11/2026.
//

#include "DSS/DistributedSS.h"

DistributedSS::~DistributedSS() {
    for (auto module: m_modules_list)
        delete module;
}

void DistributedSS::addModule(ModuleSS *module) {
    module->setParent(this);
    m_modules_list.emplace_back(module);
}

ModuleSS *DistributedSS::getModule(const size_t index) const {
    return m_modules_list.at(index);
}
