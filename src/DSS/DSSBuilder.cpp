//
// Created by chiheb on 13/07/22.
//

#include "DSSBuilder.h"

DSSBuilder::DSSBuilder(ModularPetriNet *ptr):mptrMPNet(ptr) {
    for (uint32_t i=0;i<mptrMPNet->getNbModules();i++) {
        auto elt=new ModuleSS();
        mlModuleSS.push_back(elt);
    }
}

/*
 * Build the DSS
 */
void DSSBuilder::build() {

}