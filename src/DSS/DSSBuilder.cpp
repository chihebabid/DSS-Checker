//
// Created by chiheb on 13/07/22.
//

#include "DSS/DSSBuilder.h"
#include "DSS/ArcSync.h"
#include <fstream>
#include <algorithm>

// Enable/disable reduction on the fly
//#define REDUCTION

DSSBuilder::DSSBuilder(ModularPetriNet *ptr) : m_petri(ptr) {
    for (size_t i{}; i < ModularPetriNet::getModuleCount(); ++i) {
        m_dss.addModule(new ModuleSS(i));
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
    auto productscc{new ProductSCC()};
    vector<MetaState *> list_metastates;
    for (int module {}; module < ModularPetriNet::getModuleCount(); ++module) {
        ms = m_petri->getModule(module)->getMetaState(m_petri->getModule(module)->getMarking());
        m_dss[module]->insertMS(ms);
        list_metastates.push_back(ms);
        productscc->addSCC(ms->getInitialSCC());
    }
    // Setting name for meta-states
    for (const auto lm: list_metastates)
        lm->setProductSCCName(productscc);

    vector<RElement_dss> list_fusions;
    m_petri->extractEnabledFusionReduced(list_metastates, list_fusions);
    vector<RListProductFusion> stack_fusion;
    stack_fusion.push_back(list_fusions);

    while (!stack_fusion.empty()) {
        list_fusions = stack_fusion.back();
        stack_fusion.pop_back();
        for (const auto &elt: list_fusions) {
            for (int index_global_state {}; index_global_state < elt.m_gs->size(); index_global_state++) {
                vector<Marking *> *global_state = elt.m_gs->at(index_global_state);
                TransitionFusionSet *fusion = elt.m_fusion;

                for (int module = 0; module < ModularPetriNet::getModuleCount(); ++module) {
                    if (fusion->participate(module)) {
                        m_petri->getModule(module)->setMarking((*global_state)[module]);
                    }
                    else {
                        Marking *marking = elt.getMetaState(module)->getInitialSCC()->getListStates()->at(0);
                        m_petri->getModule(module)->setMarking(marking);
                    }

                }

                fusion->fire();

                // Build obtained destination Meta-states and check whether they exist or not
                MetaState *dest_ms;
                //cout << "Build destination meta-states" << endl;
                auto *dest_productscc = new ProductSCC();
                vector<MetaState *> dest_list_metastates;
                dest_list_metastates.resize(ModularPetriNet::getModuleCount());
                for (int module = 0; module < ModularPetriNet::getModuleCount(); ++module) {
#ifdef REDUCTION
                    if (fusion->participate(module)) {
#endif

                    dest_ms =  m_petri->getModule(module)->getMetaState(m_petri->getModule(module)->getMarking());
                    dest_list_metastates[module] = dest_ms;
                    dest_productscc->addSCC(dest_ms->getInitialSCC());
#ifdef REDUCTION
                    } else dest_productscc->addSCC(elt.getMetaState(module)->getProductSCCName()->getSCC(module));
#endif
                }

                // Compute the start product
                ProductSCC startProduct;
                for (int i{}; i < global_state->size(); ++i) {
                    MetaState *_source_ms = elt.getMetaState(i);
                    if (fusion->participate(i)) {
                        SCC *_scc = _source_ms->findSCC((*global_state)[i]);
                        startProduct.addSCC(_scc);
                    }
                    else {
                        startProduct.addSCC(dest_productscc->getSCC(i));
                    }
                }
                // Check whether the computed product SCC exists or not
                vector<MetaState *> list_dest_metastates;
                bool exist = false;
                for (int module{}; module < ModularPetriNet::getModuleCount(); ++module) {
#ifdef REDUCTION
                    if (fusion->participate(module)) {
#endif
                    if (!m_dss[module]->findMetaStateByProductSCC(*dest_productscc)) {
                        auto *arc_sync{new ArcSync()};
                        MetaState *source_ms = elt.getMetaState(module);
                        arc_sync->setData(startProduct, fusion, dest_list_metastates[module]);
                        source_ms->addSyncArc(arc_sync);
                        //dest_list_metatstates.at(module)->addSyncArc(arc_sync);
                        m_dss[module]->insertMS(dest_list_metastates[module]);


                        dest_list_metastates.at(module)->setProductSCCName(dest_productscc);
                        list_dest_metastates.push_back(dest_list_metastates[module]);
                    } else {
                        exist = true;
                        auto *arc_sync{new ArcSync()};
                        MetaState *source_ms = elt.getMetaState(module);
                        MetaState *ms_dest = m_dss[module]->findMetaStateByProductSCC(*dest_productscc);
                        arc_sync->setData(startProduct, fusion, ms_dest);
                        source_ms->addSyncArc(arc_sync);
                        list_dest_metastates.push_back(ms_dest);
                    }

#ifdef REDUCTION
                    } else list_dest_metastates.push_back(elt.getMetaState(module));
#endif

                }

                if (!exist) {
                    vector<RElement_dss> new_list_fusions;
                    m_petri->extractEnabledFusionReduced(list_dest_metastates, new_list_fusions);
                    stack_fusion.push_back(new_list_fusions);
                }
            }
        }
        // Reducing
        for (const auto &elt: list_fusions) {
            for (auto module = 0; module < ModularPetriNet::getModuleCount(); ++module) {
                auto *_ms = elt.getMetaState(module);
                // _ms may itself have been merged into another metastate earlier in this loop
                const auto &alive {m_dss[module]->getListMetaStates()};
                if (std::find(alive.begin(), alive.end(), _ms) == alive.end()) continue;

                if (auto ptrMS = reduce(_ms, module);ptrMS) {
                    // Redirect edges to ptrMS
                    // A comment
                    for (const auto &m: m_dss[module]->getListMetaStates()) {
                        for (const auto &edge: m->getSyncSucc()) {
                            if (edge->getMetaStateDest() == ptrMS) edge->setDestination(_ms);
                        }
                    }
                    // _ms now also stands for the product SCCs represented by ptrMS
                    _ms->absorbNames(*ptrMS);
                    // Remove ptrMS
                    m_dss[module]->removeMetaState(ptrMS);
                }
            }
        }
    }
    // Lazy version for computing Sync-closure property
    for (size_t module{};module<ModularPetriNet::getModuleCount();++module) {
        for (auto ms: m_dss[module]->getListMetaStates()) {
            m_dss[module]->computeSyncClosedSCC(ms);
        }
    }
}


void DSSBuilder::writeToFile(const string &filename) {
    std::ofstream myfile;
    myfile.open(filename);
    myfile << "digraph " << "fichier " << "{" << endl;
    myfile << "compound=true" << endl;
    for (int module{}; module < ModularPetriNet::getModuleCount(); ++module) {
        PetriNet *petri = m_petri->getModule(module);
        for (int i {}; i < m_dss[module]->getMetaStateCount(); i++) {
            MetaState *ms = m_dss[module]->getMetaState(i);
            ProductSCC *pscc = ms->getProductSCCName();

            myfile << "subgraph cluster" << getProductSCCName(pscc) << module
                   << " {" << endl;
            /*********************************/

            for (const auto &source: ms->getListMarkings()) {
                auto sourceName = petri->getMarkingName(*source);
                myfile << sourceName << getProductSCCName(pscc);
                myfile << " [label=\"" << sourceName << "\"] ";
                myfile << ";\n";
            }

            for (const auto &source: ms->getListMarkings()) {

                std::cout<<petri->getMarkingName(*source)<<"is "<<source->getSCCContainer()->isTerminal()<<"\n";
                std::cout<<petri->getMarkingName(*source)<<"closure is "<<source->getSCCContainer()->isSyncClosed()<<"\n";
                auto sourceName = petri->getMarkingName(*source);
                auto lsucc = source->getListSucc();
                if (!lsucc.empty()) {
                    for (const auto &elt: lsucc) {
                        myfile << sourceName << getProductSCCName(pscc);
                        string tName = elt.first->getName();
                        myfile << " -> ";
                        auto destName = petri->getMarkingName(*elt.second);
                        myfile << destName << getProductSCCName(pscc);
                        myfile << "[label=\"" << tName << "\"]" << " ;" << endl;
                    }
                }
            }

            myfile << "label=\"" << getProductSCCName(pscc) << module << "\"" << endl;
            myfile << "}" << endl;

            for (const auto &arc: ms->getSyncSucc()) { // Build output edges of Metastate ms
                myfile << petri->getSCCName(ms->getProductSCCName()->getSCC(module)) << getProductSCCName(pscc)
                       << " -> ";
                MetaState *ms_dest = arc->getMetaStateDest();
                myfile
                        << petri->getSCCName(ms_dest->getProductSCCName()->getSCC(module))
                        << getProductSCCName(ms_dest->getProductSCCName());
                myfile << " ["
                       << "lhead=cluster"
                       << getProductSCCName(ms_dest->getProductSCCName())
                       << module;
                myfile << ",ltail=cluster" << getProductSCCName(pscc) << module;
                myfile << ",color=red,shape=curve,label=\"(" << getProductSCCName(arc->getStartProduct()) << "," << arc->getFusion()->getName() << ")\"]" << endl;
            }
        }
    }
    myfile << "}" << endl;
    myfile.close();
}

string DSSBuilder::getProductSCCName(ProductSCC *pss) {
    string res;
    PetriNet *petri {};
    for (int module = 0; module < ModularPetriNet::getModuleCount(); ++module) {
        petri = m_petri->getModule(module);
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
    for (const auto &elt: m_dss[module]->getListMetaStates()) {
        if (elt != ms && *elt == *ms) {
            //Compare out edges
            auto lEdges1 = ms->getSyncSucc();
            auto lEdges2 = elt->getSyncSucc();
            if (lEdges1.size() == lEdges2.size()) { // Check that all edges are the same
                bool areSame = true;
                for (const auto &edge1: lEdges1) {
                    auto compare = [&edge1](ArcSync *arc) {
                        return edge1->getFusion() == arc->getFusion() && edge1->getMetaStateDest() == arc->getMetaStateDest();
                    };
                    auto res = std::find_if(lEdges2.begin(), lEdges2.end(), compare);
                    if (res == lEdges2.end()) {
                        areSame = false;
                        break;
                    }
                }
                if (areSame) return elt;
            }
        }
    }
    return nullptr;
}

void DSSBuilder::outputTXT() {
    uint32_t sumMS{}, sumStates{}, sumSCC{}, sumLocalEdges{}, sumSyncEdges{};
    for (const auto &module: m_dss) {
        sumMS += module->getMetaStateCount();
        for (const auto &ms: module->getListMetaStates()) {
            sumStates += ms->getListMarkings().size();
            sumSCC += ms->getListSCCs()->size();
            sumLocalEdges += ms->getArcCount();
            sumSyncEdges += ms->getSyncSucc().size();
        }
    }
    cout << "#MetaStates: " << sumMS << endl;
    cout << "#States: " << sumStates << endl;
    cout << "#SCCs: " << sumSCC << endl;
    cout << "#Local edges: " << sumLocalEdges << endl;
    cout << "#Sync edges:" << sumSyncEdges << endl;
}

ModuleSS* DSSBuilder::getModuleGraph(const size_t index) const {
    return m_dss[index];
}