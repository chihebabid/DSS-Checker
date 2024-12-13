//
// Created by chiheb on 13/07/22.
//

#ifndef DISTRIBUTEDSTATESPACE_DSSBUILDER_H
#define DISTRIBUTEDSTATESPACE_DSSBUILDER_H

#include "../ModularPetriNet.h"
#include "ModuleSS.h"
#include <string>

class DSSBuilder {
public:
    DSSBuilder(ModularPetriNet *ptr);

    void build();

    void writeToFile(const string &filename);

    void outputTXT();

    ModuleSS* getModuleGraph(const size_t index) const;

private:
    string getProductSCCName(ProductSCC *pss);

    MetaState *reduce(MetaState *ms, const int &module);

    void buildInitialMS();

    ModularPetriNet *mptrMPNet;
    vector<ModuleSS *> mlModuleSS;
};


#endif //DISTRIBUTEDSTATESPACE_DSSBUILDER_H
