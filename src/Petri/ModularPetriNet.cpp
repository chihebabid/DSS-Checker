// ModularPetriNet.cpp: implementation of the ModularPetriNet class.
//
//////////////////////////////////////////////////////////////////////

#include <fstream>
#include <set>
#include "Petri/ModularPetriNet.h"
#include "time.h"
#include "DSS/ArcSync.h"


//Marking MARQUAGE_VIDE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ModularPetriNet &ModularPetriNet::getInstance() {
    static ModularPetriNet instance;
    return instance;
}

ModularPetriNet::ModularPetriNet(): m_dss(nullptr) {
    //MARQUAGE_VIDE.setEmpty(true);
}

ModularPetriNet::~ModularPetriNet() {
    for (int i = 0; i < m_fusions.size(); i++)
        delete m_fusions[i];

    for (const auto &elt: m_modules)
        delete elt;
}


void ModularPetriNet::addModule(PetriNet *petri) {
    m_modules.push_back(petri);
}

void ModularPetriNet::addSync(const string transition_name) {
    TransitionFusionSet *fusion = new TransitionFusionSet();
    fusion->setName(transition_name);
    for (const auto &petri: m_modules) {
        Transition *transition = petri->getTransitionAddress(transition_name);
        if (transition != nullptr) {
            transition->setSync(true);
            fusion->addTransition(transition);
        }
    }
    m_fusions.push_back(fusion);
}

void ModularPetriNet::setSync(vector<string> transition_codes) {
    for (auto & code : transition_codes)
        addSync(code);
}

DistributedState *ModularPetriNet::buildDSS() {
    m_dss = new DistributedState(this);
    m_dss->setModuleCount(getModuleCount());

    MetaState *ms;
    // Build initial meta-states
    //cout << "Build initial meta-states" << endl;
    ProductSCC *productscc = new ProductSCC();
    vector<MetaState *> list_metastates;
    for (int module{}; module < getModuleCount(); ++module) {
        ms = new MetaState();

        m_dss->addMetaState(ms, module);
        m_modules[module]->printMetaStateEx(ms);
        list_metastates.push_back(ms);
        productscc->addSCC(ms->getInitialSCC());
    }

    // Setting name for meta-states
    for (int i{}; i < getModuleCount(); ++i) {
        list_metastates[i]->setProductSCCName(productscc);
    }
    ListProductFusion list_fusions;
    extractEnabledFusion(productscc, list_fusions);

    vector<ListProductFusion> stack_fusion;
    stack_fusion.push_back(list_fusions);

    while (!stack_fusion.empty()) {
        list_fusions = stack_fusion.back();
        stack_fusion.pop_back();
        for (int i = 0; i < list_fusions.size(); i++) {
            Element_dss elt = list_fusions[i];
            for (int index_global_state = 0;
                 index_global_state < elt.m_gs->size();
                 index_global_state++) {
                vector<Marking *> *global_state = (*elt.m_gs)[index_global_state];
                for (int module = 0; module < getModuleCount(); module++) {
                    m_modules[module]->setMarking((*global_state)[module]);
                }
                TransitionFusionSet *fusion = elt.m_fusion;
                fusion->fire();

                // Build obtained destination Meta-states and check whether they exist or not
                MetaState *dest_ms;
                // Build initial meta-states
                //cout << "Build destination meta-states" << endl;
                ProductSCC *dest_productscc = new ProductSCC();
                vector<MetaState *> dest_list_metastates;
                for (int module = 0; module < getModuleCount(); module++) {
                    dest_ms = new MetaState();

                    dest_list_metastates.push_back(dest_ms);
                    dest_productscc->addSCC(dest_ms->getInitialSCC());
                }
                // cout << "Product built : " << getProductSCCName(dest_productscc)<<endl;

                // Check whether the computed product SCC exists or not
                if (!m_dss->getMetaGraph(0)->findMetaStateByProductSCC(
                    dest_productscc)) {
                    // Insert metastate into DSS
                    for (int module = 0; module < getModuleCount(); module++) {
                        auto arc_sync = std::make_unique<ArcSync>();
                        MetaState *source_ms =
                                m_dss->getMetaGraph(module)->findMetaStateByProductSCC(
                                    elt.m_product);
                        ProductSCC startProduct;
                        for (int i = 0; i < global_state->size(); i++) {
                            MetaState *_source_ms =
                                    m_dss->getMetaGraph(i)->findMetaStateByProductSCC(
                                        elt.m_product);
                            SCC *_scc = _source_ms->findSCC(
                                (*global_state)[i]);
                            // "any" if module i is not synchronised on fusion
                            startProduct.addSCC(_scc, !fusion->participate(i));
                        }
                        arc_sync->setData(startProduct, fusion,
                                          dest_list_metastates[module]);
                        source_ms->addSyncArc(std::move(arc_sync));
                        m_dss->addMetaState(dest_list_metastates[module],
                                            module);
                    }

                    // Setting name for meta-states
                    for (int i = 0; i < getModuleCount(); i++) {
                        dest_list_metastates[i]->setProductSCCName(
                            dest_productscc);
                    }
                    ListProductFusion new_list_fusions;
                    extractEnabledFusion(dest_productscc, new_list_fusions);
                    stack_fusion.push_back(new_list_fusions);
                } else {
                    for (int module = 0; module < getModuleCount(); module++) {
                        auto arc_sync = std::make_unique<ArcSync>();
                        MetaState *source_ms =
                                m_dss->getMetaGraph(module)->findMetaStateByProductSCC(elt.m_product);

                        ProductSCC startProduct;
                        for (int i = 0; i < global_state->size(); i++) {
                            MetaState *_source_ms = m_dss->getMetaGraph(i)->findMetaStateByProductSCC(elt.m_product);
                            SCC *_scc = _source_ms->findSCC((*global_state)[i]);
                            // "any" if module i is not synchronised on fusion
                            startProduct.addSCC(_scc, !fusion->participate(i));
                        }
                        MetaState *ms_dest = m_dss->getMetaGraph(module)->findMetaStateByProductSCC(dest_productscc);
                        arc_sync->setData(startProduct, fusion, ms_dest);
                        source_ms->addSyncArc(std::move(arc_sync));
                    }
                }
            }
        }
    }

    return m_dss;
}

void ModularPetriNet::extractEnabledFusionReduced(vector<MetaState *> &list_ms,
                                                  vector<RElement_dss> &list_elt) {
    vector<ListLocalStates> states_enabling_fusion;

    for (int index_fusion = 0; index_fusion < m_fusions.size(); index_fusion++) {
        /************************************************/
        /**Check whether a fusion set is enabled or not**/
        /************************************************/
        TransitionFusionSet *fusion = m_fusions[index_fusion];
        bool canBeActive = true;
        for (int j = 0; j < getModuleCount() && canBeActive; j++) {
            if (fusion->participate(j) && fusion->participatePartially(j)) {
                canBeActive = false;
                auto list_states = list_ms[j]->getListMarkings(); //  m_dss->getLocalStates(product,j);
                for (int index = 0; index < list_states.size() && !canBeActive; index++) {
                    m_modules[j]->setMarking(list_states[index]);
                    if (fusion->isLocallyFirable(j)) {
                        canBeActive = true;
                    }
                }
            }
        }

        if (canBeActive) {
            // Compute local markings that enable the transition fusion set
            states_enabling_fusion.resize(getModuleCount());
            for (int j = 0; j < getModuleCount(); j++) {
                auto list_states = list_ms[j]->getListMarkings();
                // states_enabling_fusion.at(j)= NULL;
                states_enabling_fusion[j] = new vector<Marking *>();
                if (fusion->participate(j)) {
                    for (int index = 0; index < list_states.size(); index++) {
                        m_modules[j]->setMarking(list_states[index]);
                        if (fusion->isLocallyFirable(j)) {
                            states_enabling_fusion[j]->push_back(list_states[index]);
                        }
                    }
                } else if (!fusion->participate(j)) {
                    states_enabling_fusion[j]->push_back(list_states[0]);
                }
            }
            ListGlobalStates list_globalstates = computeSynchronizedProduct(states_enabling_fusion);
            RElement_dss elt;
            elt.m_gs = list_globalstates;
            elt.m_fusion = fusion;
            elt.setListMetaStates(list_ms);
            list_elt.push_back(elt);
        }
    }
}


void ModularPetriNet::extractEnabledFusion(ProductSCC *product,
                                           vector<Element_dss> &list_elt) {
    vector<ListLocalStates> states_enabling_fusion;

    for (const auto &fusion: m_fusions) {
        /************************************************/
        /**Check whether a fusion set is enabled or not**/
        /************************************************/

        bool canBeActive = true;
        for (int j = 0; j < getModuleCount() && canBeActive; j++) {
            if (fusion->participate(j) && fusion->participatePartially(j)) {
                canBeActive = false;
                auto list_states = m_dss->getLocalStates(product, j);
                for (int index = 0; index < list_states.size() && !canBeActive; index++) {
                    m_modules[j]->setMarking(list_states[index]);
                    if (fusion->isLocallyFirable(j)) {
                        canBeActive = true;
                    }
                }
            }
        }

        if (canBeActive) {
            //////////////////////////////////////////////////////////////////////////
            // D�terminer tous les marquages locaux activant la fusion de transitions
            //////////////////////////////////////////////////////////////////////////

            states_enabling_fusion.resize(getModuleCount());
            for (int j = 0; j < getModuleCount(); j++) {
                auto list_states {m_dss->getLocalStates(product, j)};
                states_enabling_fusion[j] = new vector<Marking *>();
                if (fusion->participate(j)) {
                    for (int index = 0; index < list_states.size(); index++) {
                        m_modules[j]->setMarking(list_states[index]);
                        if (fusion->isLocallyFirable(j)) {
                            states_enabling_fusion[j]->push_back(list_states[index]);
                        }
                    }
                } else if (!fusion->participate(j)) {
                    states_enabling_fusion[j]->push_back(list_states[0]);
                }
            }

            ListGlobalStates list_globalstates = computeSynchronizedProduct(states_enabling_fusion);
            Element_dss elt;
            elt.m_gs = list_globalstates;
            elt.m_fusion = fusion;
            elt.m_product = product;
            list_elt.push_back(elt);
        }
    }
}

ListGlobalStates ModularPetriNet::computeSynchronizedProduct(vector<ListLocalStates> &states_enabling_fusion) {
    vector<int> indices;
    indices.resize(getModuleCount());
    fill(indices.begin(), indices.end(), 0);

    ListGlobalStates global_states = new vector<ListLocalStates>();
    bool ajouter;
    bool result;
    bool first = true;

    do {
        ajouter = true;
        for (int i = 0; i < states_enabling_fusion.size() && ajouter; i++) {
            if (!first) indices[i] += 1;
            else first = false;
            if (indices[i] == states_enabling_fusion[i]->size())
                indices[i] = 0;
            else
                ajouter = false;
        }
        //

        vector<Marking *> *local_states = new vector<Marking *>();
        for (int i = 0; i < states_enabling_fusion.size(); i++) {
            local_states->push_back(states_enabling_fusion[i]->at(indices[i]));
        }
        global_states->push_back(local_states);
        //Vérifier si on a aprcouru toutes les combinaisons
        result = true;
        for (int i = 0; i < states_enabling_fusion.size() && result; i++) {
            if ((states_enabling_fusion[i]->size() - 1) != indices[i]) result = false;
        }
    } while (result == false);
    return global_states;
}

void ModularPetriNet::writeToFile(const string filename) {
    std::ofstream myfile;
    myfile.open(filename);
    myfile << "digraph " << "fichier " << "{" << endl;
    myfile << "compound=true" << endl;
    for (int module = 0; module < getModuleCount(); module++) {
        PetriNet *petri = m_modules[module];

        for (int i = 0; i < m_dss->getMetaGraph(module)->getMetaStateCount();
             i++) {
            MetaState *ms = m_dss->getMetaGraph(module)->getMetaState(i);
            ProductSCC *pscc = ms->getProductSCCName();

            myfile << "subgraph cluster" << getProductSCCName(pscc) << module
                    << " {" << endl;
            /*for (int jj = 0; jj < ms->getListArcs()->size(); jj++) {
                Marking *source_marking = ms->getListArcs()->at(jj).getSource();
                Marking *dest_marking =
                        ms->getListArcs()->at(jj).getDestination();
                myfile << petri->getMarkingName(*source_marking)
                        << getProductSCCName(pscc) << module << " -> ";
                myfile << petri->getMarkingName(*dest_marking)
                        << getProductSCCName(pscc) << module;
                myfile << " [label=\""
                        << ms->getListArcs()->at(jj).getTransition()->getName()
                        << "\"]" << endl;
            }*/
            myfile << "label=\"" << getProductSCCName(pscc) << "\"" << endl;
            myfile << "}" << endl;

            for (int k = 0; k < ms->getSyncSucc().size(); k++) {
                myfile << petri->getSCCName(pscc->getSCC(module))
                        << getProductSCCName(pscc) << module << " -> ";
                ArcSync *arc = ms->getSyncSucc().at(k).get();
                MetaState *ms_dest = arc->getDestination();
                myfile
                        << petri->getSCCName(
                            ms_dest->getProductSCCName()->getSCC(module))
                        << getProductSCCName(ms_dest->getProductSCCName())
                        << module;
                myfile << " [ltail=cluster" << getProductSCCName(pscc) << module
                        << ",lhead=cluster"
                        << getProductSCCName(ms_dest->getProductSCCName())
                        << module << "]" << endl;
                //myfile<<arc->getFusion()->getName()<<"]";
                //myfile<<endl;
            }
        }
    }
    myfile << "}" << endl;
    myfile.close();
}

string ModularPetriNet::getProductSCCName(const ProductSCC *pss) {
    string res = "";
    for (int module = 0; module < getModuleCount(); module++) {
        PetriNet *petri = m_modules.at(module);
        res += petri->getSCCName(pss->getSCC(module));
    }
    return res;
}

void ModularPetriNet::writeTextFile(const string filename) {
    std::ofstream myfile;
    myfile.open(filename);

    for (int module = 0; module < getModuleCount(); module++) {
        myfile << "******** Module " << module << " ********" << endl;
        myfile << "#Meta-states : "
                << m_dss->getMetaGraph(module)->getMetaStateCount() << endl;
        PetriNet *petri = m_modules.at(module);

        for (int i = 0; i < m_dss->getMetaGraph(module)->getMetaStateCount(); i++) {
            MetaState *ms = m_dss->getMetaGraph(module)->getMetaState(i);
            ProductSCC *pscc = ms->getProductSCCName();
            myfile << "Metastate : " << getProductSCCName(pscc) << endl;
            for (auto source_mark: ms->getListMarkings()) {
                myfile << petri->getMarkingName(*source_mark);
                for (auto succ: source_mark->getListSucc()) {
                    Transition *t = succ.first;
                    Marking *dest_marking = succ.second;

                    myfile << " - " << t->getName() << " -> ";
                    myfile << petri->getMarkingName(*dest_marking) << endl;
                }
            }
            myfile << endl;
        }
        // Compute #(sync arcs)
        unsigned int total_arcs = 0;
        for (int i = 0; i < m_dss->getMetaGraph(module)->getMetaStateCount(); i++) {
            MetaState *ms = m_dss->getMetaGraph(module)->getMetaState(i);
            total_arcs += ms->getSyncSucc().size();
        }
        myfile << "#Synchronisation arcs : " << total_arcs << endl;
        // List sync arcs
        for (int i = 0; i < m_dss->getMetaGraph(module)->getMetaStateCount(); i++) {
            MetaState *ms = m_dss->getMetaGraph(module)->getMetaState(i);
            ProductSCC *pscc = ms->getProductSCCName();
            for (int k = 0; k < ms->getSyncSucc().size(); k++) {
                myfile << getProductSCCName(pscc) << " ==== ";
                ArcSync *arc = ms->getSyncSucc()[k].get();
                MetaState *ms_dest = arc->getDestination();
                myfile << "(" << getProductSCCName(&arc->getSourceProduct())
                        << "," << arc->getFusion()->getName() << ") ===> ";
                myfile << getProductSCCName(ms_dest->getProductSCCName()) << endl;
                //myfile<<arc->getFusion()->getName()<<"]";
                //myfile<<endl;
            }
        }
    }

    myfile.close();
}

/*
 * Returns the module to which all transitions belong
 *
 */
size_t ModularPetriNet::getModule(const std::set<string> &list_transitions) {
    size_t index{0};
    for (const auto &module: m_modules) {
        if (module->areTransitionsIncluded(list_transitions)) return index;
        ++index;
    }
    return -1; // Error
}

/*
 * Returns a Petri  net giving its index
 */
PetriNet *ModularPetriNet::getModule(const int index) {
    return m_modules[index];
}

size_t ModularPetriNet::getModuleCount() {
    return getInstance().m_modules.size();
}