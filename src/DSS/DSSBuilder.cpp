//
// Created by chiheb on 13/07/22.
//

#include "DSSBuilder.h"
#include "ArcSync.h"
#include <fstream>

// Enable/disable reduction on the fly
//#define REDUCTION

DSSBuilder::DSSBuilder(ModularPetriNet *ptr) : mptrMPNet(ptr) {
    for (uint32_t i = 0; i < mptrMPNet->getNbModules(); i++) {
        auto elt = new ModuleSS();
        mlModuleSS.push_back(elt);
    }
}

/*
 * @brief Build the DSS
 */
void DSSBuilder::build() {
    buildInitialMS();
}

/*
 * @brief Build initial MS
 */
void DSSBuilder::buildInitialMS() {
    MetaState *ms;
    // Build initial meta-states
    cout << "Build initial meta-states" << endl;
    auto *productscc = new ProductSCC();
    vector<MetaState *> list_metatstates;
    for (int module = 0; module < mptrMPNet->getNbModules(); ++module) {
        ms = new MetaState();
        StateGraph *state_graph = mptrMPNet->getModule(module)->getStateGraph(mptrMPNet->getModule(module)->getMarquage());
        state_graph->setID(module);
        ms->setStateGraph(state_graph);
        mlModuleSS[module]->insertMS(ms);
        list_metatstates.push_back(ms);
        productscc->addSCC(ms->getInitialSCC());
    }
    // Setting name for meta-states
    for (const auto lm: list_metatstates)
        lm->setSCCProductName(productscc);

    vector<RElement_dss> list_fusions;
    mptrMPNet->extractEnabledFusionReduced(list_metatstates, list_fusions);
    vector<RListProductFusion> stack_fusion;
    stack_fusion.push_back(list_fusions);

    while (!stack_fusion.empty()) {
        list_fusions = stack_fusion.back();
        stack_fusion.pop_back();
        for (const auto &elt: list_fusions) {
            for (int index_global_state = 0; index_global_state < elt.m_gs->size(); index_global_state++) {
                vector<Marking *> *global_state = elt.m_gs->at(index_global_state);
                for (int module = 0; module < mptrMPNet->getNbModules(); ++module) {
                    mptrMPNet->getModule(module)->setMarquage(global_state->at(module));
                }
                Fusion *fusion = elt.m_fusion;
                fusion->tirer();

                // Build obtained destination Meta-states and check whether they exist or not
                MetaState *dest_ms;
                //cout << "Build destination meta-states" << endl;
                auto *dest_productscc = new ProductSCC();
                vector<MetaState *> dest_list_metatstates;
                dest_list_metatstates.resize(mptrMPNet->getNbModules());
                for (int module = 0; module < mptrMPNet->getNbModules(); module++) {
#ifdef REDUCTION
                    if (fusion->participate(module)) {
#endif
                    dest_ms = new MetaState();
                    StateGraph *state_graph = mptrMPNet->getModule(module)->getStateGraph(
                            mptrMPNet->getModule(module)->getMarquage());
                    state_graph->setID(module);
                    dest_ms->setStateGraph(state_graph);
                    dest_list_metatstates[module] = dest_ms;
                    dest_productscc->addSCC(dest_ms->getInitialSCC());
#ifdef REDUCTION
                    } else dest_productscc->addSCC(elt.getMetaState(module)->getSCCProductName()->getSCC(module));
#endif
                }

                // Compute the start product
                ProductSCC startProduct;
                for (int i = 0; i < global_state->size(); i++) {
                    MetaState *_source_ms = elt.getMetaState(i);
                    SCC *_scc = _source_ms->findSCC(global_state->at(i));
                    startProduct.addSCC(_scc);
                }
                // Check whether the computed product SCC exists or not

                vector<MetaState *> list_dest_metatstates;
                bool exist = false;
                for (int module = 0; module < mptrMPNet->getNbModules(); module++) {
#ifdef REDUCTION
                    if (fusion->participate(module)) {
#endif
                    if (!mlModuleSS[module]->findMetaStateByProductSCC(*dest_productscc)) {
                        ArcSync *arc_sync = new ArcSync();
                        MetaState *source_ms = elt.getMetaState(module);
                        arc_sync->setData(startProduct, fusion, dest_list_metatstates.at(module));
                        source_ms->addSyncArc(arc_sync);
                        //dest_list_metatstates.at(module)->addSyncArc(arc_sync);
                        mlModuleSS[module]->insertMS(dest_list_metatstates.at(module));

                        //m_modules[module]->printMetaStateEx(
                        //		dest_list_metatstates.at(module));
                        dest_list_metatstates.at(module)->setSCCProductName(dest_productscc);
                        list_dest_metatstates.push_back(dest_list_metatstates.at(module));
                    } else {
                        exist = true;
                        auto *arc_sync = new ArcSync();
                        MetaState *source_ms = elt.getMetaState(module);
                        MetaState *ms_dest = mlModuleSS[module]->findMetaStateByProductSCC(*dest_productscc);
                        arc_sync->setData(startProduct, fusion, ms_dest);
                        source_ms->addSyncArc(arc_sync);
                        list_dest_metatstates.push_back(ms_dest);
                    }

#ifdef REDUCTION
                    } else list_dest_metatstates.push_back(elt.getMetaState(module));
#endif

                }

                if (!exist) {
                    vector<RElement_dss> new_list_fusions;
                    mptrMPNet->extractEnabledFusionReduced(list_dest_metatstates, new_list_fusions);
                    stack_fusion.push_back(new_list_fusions);
                }
            }
        }
        // Reducing
        for (const auto &elt: list_fusions) {
            for (auto module = 0; module < mptrMPNet->getNbModules(); ++module) {
                auto *ms = elt.getMetaState(module);
                auto ptrMS=reduce(ms, module);
                if (ptrMS) {
                    // Redirect edges to ptrMS
                    // Remove ptrMS
                    mlModuleSS[module]->removeMetaState(ptrMS);
                }
            }
        }
    }
}


void DSSBuilder::writeToFile(const string &filename) {
    std::ofstream myfile;
    myfile.open(filename);
    myfile << "digraph " << "fichier " << "{" << endl;
    myfile << "compound=true" << endl;
    for (int module = 0; module < mptrMPNet->getNbModules(); ++module) {
        PetriNet *petri = mptrMPNet->getModule(module);
        for (int i = 0; i < mlModuleSS[module]->getMetaStateCount(); i++) {
            MetaState *ms = mlModuleSS[module]->getMetaState(i);
            ProductSCC *pscc = ms->getSCCProductName();
            myfile << "subgraph cluster" << getProductSCCName(pscc) << module
                   << " {" << endl;
            /*********************************/
            StateGraph *ss = ms->getStateGraph();
            for (const auto &source: *ss->getListMarquages()) {
                auto sourceName = petri->getMarquageName(*source);
                myfile << sourceName << getProductSCCName(pscc);
                myfile << " [label=\"" << sourceName << "\"] ";
                myfile << ";\n";
            }

            for (const auto &source: *(ss->getListMarquages())) {
                auto sourceName = petri->getMarquageName(*source);
                auto lsucc = source->getListSucc();
                if (!lsucc->empty()) {
                    for (const auto &elt: *lsucc) {
                        myfile << sourceName << getProductSCCName(pscc);
                        string tName = elt.first->getName();
                        myfile << " -> ";
                        auto destName = petri->getMarquageName(*elt.second);
                        myfile << destName << getProductSCCName(pscc);
                        myfile << "[label=\"" << tName << "\"]" << " ;" << endl;
                    }
                }
            }

            myfile << "label=\"" << getProductSCCName(pscc) << module << "\"" << endl;
            myfile << "}" << endl;

            for (int k = 0; k < ms->getSucc().size(); k++) {
                //myfile << petri->getSCCName(pscc->getSCC(module))

                // myfile<< getProductSCCName(pscc) << module << " -> ";
                //myfile<<ms->
                ArcSync *arc = ms->getSucc().at(k);
                // myfile<<getProductSCCName(arc->getStartProduct())->getSCC(module)<<" -> ";
                myfile << petri->getSCCName(ms->getSCCProductName()->getSCC(module)) << getProductSCCName(pscc)
                       << " -> ";
                MetaState *ms_dest = arc->getMetaStateDest();
                myfile
                        << petri->getSCCName(
                                ms_dest->getSCCProductName()->getSCC(module))
                        << getProductSCCName(ms_dest->getSCCProductName());
                /*myfile << " [ltail=cluster" << getProductSCCName(pscc) << module
                       << ",lhead=cluster"
                       << getProductSCCName(ms_dest->getSCCProductName())
                       << module << "]" << endl;*/

                myfile << " ["
                       << "lhead=cluster"
                       << getProductSCCName(ms_dest->getSCCProductName())
                       << module << ",color=red,shape=curve,label=\"" << arc->getFusion()->getName() << "\"]" << endl;
                //myfile<<arc->getFusion()->getName()<<"]";
                //myfile<<endl;

            }
        }
    }
    myfile << "}" << endl;
    myfile.close();
}

string DSSBuilder::getProductSCCName(ProductSCC *pss) {
    string res;
    for (int module = 0; module < mptrMPNet->getNbModules(); ++module) {
        PetriNet *petri = mptrMPNet->getModule(module);
        res += petri->getSCCName(pss->getSCC(module));
    }
    return res;
}

/*
 * @brief Check whether a metastate can be fused with another
 * @param ms a Metastate
 * @param module Module index
 * @return true if *ms can be fusedm else false
 */
MetaState *DSSBuilder::reduce(MetaState *ms, const int &module) {

    for (const auto &elt: mlModuleSS[module]->getLMetaState()) {
        if (elt != ms && *elt == *ms) {
            //Compare out edges
            auto lEdges1 = ms->getSucc();
            auto lEdges2 = elt->getSucc();
            if (lEdges1.size() == lEdges2.size()) { // Check that all edges are the same
                bool areSame=true;
                for (const auto &edge1: lEdges1) {
                    auto compare = [&edge1](ArcSync *arc) {
                        return edge1->getFusion() == arc->getFusion() &&  edge1->getMetaStateDest() == arc->getMetaStateDest();
                    };
                    auto res = std::find_if(lEdges2.begin(), lEdges2.end(), compare);
                    if (res == lEdges2.end()) {
                        areSame= false;
                        break;
                    }
                }
                if (areSame) return elt;
            }
        }
    }
    return nullptr;
}