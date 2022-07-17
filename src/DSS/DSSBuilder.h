//
// Created by chiheb on 13/07/22.
//

#ifndef DISTRIBUTEDSTATESPACE_DSSBUILDER_H
#define DISTRIBUTEDSTATESPACE_DSSBUILDER_H
#include "ModularPetriNet.h"
#include "ModuleSS.h"
#include <string>

class DSSBuilder {
public:
    DSSBuilder(ModularPetriNet *ptr);
    void build();
    void writeToFile(const string& filename);
private:
    string getProductSCCName(ProductSCC *pss);
    void buildInitialMS();
    ModularPetriNet *mptrMPNet;
    vector<ModuleSS*> mlModuleSS;
};


#endif //DISTRIBUTEDSTATESPACE_DSSBUILDER_H
