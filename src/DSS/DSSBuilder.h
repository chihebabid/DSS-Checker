//
// Created by chiheb on 13/07/22.
//

#ifndef DISTRIBUTEDSTATESPACE_DSSBUILDER_H
#define DISTRIBUTEDSTATESPACE_DSSBUILDER_H
#include "ModularPetriNet.h"
#include "ModuleSS.h"

class DSSBuilder {
public:
    DSSBuilder(ModularPetriNet *ptr);
    void build();
private:
    void buildInitialMS();
    ModularPetriNet *mptrMPNet;
    vector<ModuleSS*> mlModuleSS;
};


#endif //DISTRIBUTEDSTATESPACE_DSSBUILDER_H
