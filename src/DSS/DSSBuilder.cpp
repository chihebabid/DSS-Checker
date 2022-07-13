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
    buildInitialMS();
}

/*
 * Build initial MS
 */
void DSSBuilder::buildInitialMS() {
    MetaState *ms;
    // Build initial meta-states
    cout << "Build initial meta-states" << endl;
    ProductSCC * productscc = new ProductSCC();
    vector<MetaState*> list_metatstates;
    for (int module = 0; module < mptrMPNet->getNbModules(); ++module) {
        ms = new MetaState();
        /*StateGraph *state_graph = m_modules[module]->getStateGraph(
                m_modules[module]->getMarquage());
        state_graph->setID(module);
        ms->setStateGraph(state_graph);
        m_dss->addMetaState(ms, module);
        //m_modules[module]->printMetaStateEx(ms);
        list_metatstates.push_back(ms);
        productscc->addSCC(ms->getInitialSCC());*/
    }


}