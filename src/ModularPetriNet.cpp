// ModularPetriNet.cpp: implementation of the ModularPetriNet class.
//
//////////////////////////////////////////////////////////////////////

//#include "StdAfx.h"
#include "Element_dss.h"
#include "ModularPetriNet.h"
#include "time.h"
#include "ArcSync.h"
#include <fstream>

Marking MARQUAGE_VIDE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
ModularPetriNet::ModularPetriNet() {
    MARQUAGE_VIDE.setVide(true);
    m_espace = nullptr;
    m_graphe_sync = nullptr;
    m_dss = nullptr;
}

ModularPetriNet::~ModularPetriNet() {
    for (int i = 0; i < m_fusions.size(); i++)
        delete m_fusions[i];
    m_fusions.clear();
}


void ModularPetriNet::addModule(PetriNet *petri) {
    m_modules.push_back(petri);
}

void ModularPetriNet::addSync(const string transition_name) {
    PetriNet *petri;
    Fusion *fusion = new Fusion();
    fusion->setName(transition_name);
    for (int i = 0; i < m_modules.size(); i++) {
        petri = m_modules[i];
        Transition *transition = petri->getTransitionAdresse(transition_name);
        if (transition != NULL) {
            transition->setSync(true);
            fusion->addTransition(transition);
        }

    }
    m_fusions.push_back(fusion);
}

void ModularPetriNet::setSync(vector<string> liste_code_transitions) {
    for (int i = 0; i < liste_code_transitions.size(); i++)
        addSync(liste_code_transitions[i]);
}

int ModularPetriNet::getCountFusions() {
    return m_fusions.size();
}

Fusion *ModularPetriNet::getFusion(const int index) {
    return m_fusions[index];
}

vector<Fusion *> ModularPetriNet::getFusionsFranchissables() {
    vector<Fusion *> liste;
    for (int i = 0; i < m_fusions.size(); i++) {
        if (m_fusions[i]->isFranchissable())
            liste.push_back(m_fusions[i]);
    }
    return liste;
}

void ModularPetriNet::printMarquage() {
    for (int i = 0; i < getModulesCount(); i++) {
        m_modules[i]->printMarquage();
    }
}

////////////////////////////////////////////////////
/// Construire un espace d'�tats modulaire r�duit///
////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////
// Renommer les transitions internes � l'exception de celles pass�es en param�tre //
////////////////////////////////////////////////////////////////////////////////////
void ModularPetriNet::renommerTransitions(vectorString liste_transitions) {
    for (int i = 0; i < m_modules.size(); i++)
        m_modules.at(i)->renommerTransitions(liste_transitions);
}

//////////////////////////////////////////
// R�duire le nom d'un marquage global
/////////////////////////////////////
void ModularPetriNet::reduceMarquageName(NodeSG *marquage_global_depart,
                                         Fusion *fusion) {
    for (int i = 0; i < getModulesCount(); i++) {
        if (!fusion->participate(i)) {
            marquage_global_depart->setVide(i);
        }
    }
}

///////////////////////////////////////////////////////////////////////
// Retourner le marquage courant � un module
///////////////////////////////////////////////////////////////////////
Marking ModularPetriNet::getMarquageModule(const int index) {
    return m_modules[index]->getMarquage();
}

///////////////////////////////////////////////////////////////////////////
// Une nouvelle version de la construction du graphe de synchronisation
///////////////////////////////////////////////////////////////////////////
void ModularPetriNet::constructSync2() {
    m_espace->setGrapheSync(m_graphe_sync);
    // Contient les marquages locaux obtenus � partir des espaces locaux
    vector<ListMarqLoc *> list_marq_locaux;

    // Initialiser le premier noeud du SG
    NodeSG *noeudSG_start = new NodeSG();
    list_marq_locaux.resize(getModulesCount());

    // Parcourir les espaces d'�tats locaux
    for (int module = 0; module < getModulesCount(); module++) {
        ListMarquage *noeud_equiv = NULL;
        list_marq_locaux[module] = m_modules[module]->getListMarquageAccFrom(
                m_modules[module]->getMarquage(),
                m_espace->getEspaceLocal(module), &noeud_equiv);
        noeudSG_start->addMarquage(list_marq_locaux[module]->at(0),
                                   noeud_equiv);
    }

    /*********************************************************/
    /**Ins�rer le premier noeud au graphe de synchronisation**/
    /*********************************************************/
    m_graphe_sync->addNode(noeudSG_start);
    extractionFusion(list_marq_locaux, noeudSG_start);
    // Sauvegarder ds la pile
    int index_node = 0;

    do {
        NodeSG *noeud = m_graphe_sync->getNode(index_node);
        //cout<<index_node<<" , "<<m_graphe_sync->getNodesCount()<<"\n";
        //m_espace->display();
        /////////////////////////////////////

        for (int index = 0; index < noeud->getCountArcs(); index++) {
            for (int module = 0; module < getModulesCount(); module++) {
                Marking source =
                        *(*noeud->getArc(index)->getMarquageSource()).getMarquage(
                                module);
                m_modules[module]->setMarquage(&source);
            }
            Fusion *fusion = noeud->getArc(index)->getFusion();
            fusion->tirer();

            NodeSG *noeudSG_arrivee = new NodeSG();
            for (int module = 0; module < getModulesCount(); module++) {
                Marking m = m_modules.at(module)->getMarquage();
                noeudSG_arrivee->addMarquage(m, NULL);
            }

            NodeSG *node_arrive_nvo = m_graphe_sync->addNode(noeudSG_arrivee);

            if (node_arrive_nvo == noeudSG_arrivee) {

                //noeudSG_arrivee->m_list.clear();
                //noeudSG_arrivee->m_equiv.clear();
                for (int module = 0; module < getModulesCount(); module++) {
                    ListMarquage *noeud_equiv = NULL;
                    if (fusion->participate(module)
                        || !(list_marq_locaux[module]->at(0)
                             == m_modules[module]->getMarquage())) {
                        delete list_marq_locaux[module];

                        list_marq_locaux[module] =
                                m_modules[module]->getListMarquageAccFrom(
                                        m_modules[module]->getMarquage(),
                                        m_espace->getEspaceLocal(module),
                                        &noeud_equiv);
                        noeudSG_arrivee->m_equiv[module] = noeud_equiv;

                    } else {
                        noeudSG_arrivee->m_equiv[module] =
                                m_espace->getEspaceLocal(module)->getFirstNodeContainMarquage(
                                        noeud->getMarquage(module));
                    }

                }
                extractionFusion(list_marq_locaux, noeudSG_arrivee);

            } else
                delete noeudSG_arrivee;
            ArcSG *arc_sg = noeud->getArc(index);
            arc_sg->setDestination(node_arrive_nvo);
            for (int module = 0; module < getModulesCount(); module++) {
                ListMarquage *noeud_local =
                        m_espace->getEspaceLocal(module)->getFirstNodeContainMarquage(
                                noeud->getArc(index)->getMarquageSource()->getMarquage(
                                        module));
                if (noeud_local != NULL) {
                    *(arc_sg->getMarquageSource()->getMarquage(module)) =
                            *(noeud_local->getMarquage(0));
                }
            }

        }

        Operations::reduceArcs(&noeud->m_arcs);
        //noeud->m_marked=true;
        index_node++;

    } while (index_node < m_graphe_sync->getNodesCount());
}

/////////////////////////////////////////////////////////////
// Calcul du produit synchronis� d'un ensemble de marq locaux
//////////////////////////////////////////////////////////////
vector<NodeSGSimplified> *ModularPetriNet::calculerProduitSynchronises(
        vector<ListMarqLoc> &liste) {
    vector<int> indices;
    indices.resize(getModulesCount());
    fill(indices.begin(), indices.end(), 0);

    vector<NodeSGSimplified> *myliste = new vector<NodeSGSimplified>();
    bool ajouter;
    bool result;
    bool first = true;

    do {

        ajouter = true;
        for (int i = 0; i < liste.size() && ajouter; i++) {
            if (!first)
                indices[i] += 1;
            else
                first = false;
            if (indices[i] == liste[i].size())
                indices[i] = 0;
            else
                ajouter = false;
        }
        //

        NodeSGSimplified marq;
        for (int i = 0; i < liste.size(); i++) {
            marq.addMarquage(liste[i].at(indices[i]));
        }
        myliste->push_back(marq);
        //V�rifier si on a aprcouru toutes les combinaisons
        result = true;
        for (int i = 0; i < liste.size() && result; i++) {
            if ((liste[i].size() - 1) != indices[i])
                result = false;
        }
    } while (result == false);
    return myliste;
}

void ModularPetriNet::extractionFusion(vector<ListMarqLoc *> &liste_,
                                       NodeSG *node) {

    vector<int> indices;
    indices.resize(getModulesCount());

    fill(indices.begin(), indices.end(), 0);

    for (int index_fusion = 0; index_fusion < m_fusions.size();
         index_fusion++) {
        /****************************************************************/
        /****************** Tester si la fusion de transitions est franchissables *************************/
        /****************************************************************/
        Fusion *fusion = m_fusions.at(index_fusion);
        bool canBeActive = true;
        for (int j = 0; j < getModulesCount() && canBeActive; j++) {
            if (fusion->participate(j) && fusion->participatePartially(j)) {
                canBeActive = false;
                for (int index = 0;
                     index < liste_.at(j)->size() && !canBeActive; index++) {
                    m_modules.at(j)->setMarquage(&liste_.at(j)->at(index));
                    if (fusion->isFranchissableLocal(j)) {
                        canBeActive = true;
                    }

                }
            }
        }

        if (canBeActive) {
            //////////////////////////////////////////////////////////////////////////
            // D�terminer tous les marquages locaux activant la fusion de transitions
            //////////////////////////////////////////////////////////////////////////
            vector<ListMarqLoc> liste_marq_locaux;
            liste_marq_locaux.resize(getModulesCount());
            for (int j = 0; j < getModulesCount(); j++) {
                //liste_marq_locaux[j]=new ListMarqLoc();
                if (fusion->participate(j)) {
                    for (int index = 0; index < liste_.at(j)->size(); index++) {
                        m_modules.at(j)->setMarquage(&liste_.at(j)->at(index));
                        if (fusion->isFranchissableLocal(j)) {
                            liste_marq_locaux[j].push_back(
                                    liste_.at(j)->at(index));
                        }

                    }

                } else if (!fusion->participate(j)) {

                    Marking m = liste_.at(j)->at(0);
                    m.setVide(true);
                    liste_marq_locaux[j].push_back(m);

                } else {
                    liste_marq_locaux[j].resize(liste_.at(j)->size());
                    copy(liste_.at(j)->begin(), liste_.at(j)->end(),
                         liste_marq_locaux[j].begin());

                }
            }

            vector<NodeSGSimplified> *liste_node = calculerProduitSynchronises(
                    liste_marq_locaux);
            for (unsigned int i = 0; i < liste_node->size(); i++) {
                node->addArc(liste_node->at(i), fusion, nullptr);
            }

        }
    }

}

ModularSpace *ModularPetriNet::constructReducedStateSpace() {
    m_graphe_sync = new GrapheSync();
    m_espace = new ModularSpace();
    m_espace->setParent(this);
    m_espace->setNbModules(getModulesCount());
    // Cr�ation des espaces d'�tats locaux
    for (int index_module = 0; index_module < getModulesCount(); index_module++) {
        Automata *automata = new Automata();
        automata->setParent(m_espace, index_module);
        m_espace->insertGraphe(index_module, automata);
    }
    // 1�re �tape : Construction du graphe de synchronisation et des graphes locaux incomplets

    constructSync2();

    // 2�me �tape : Ach�vement de la construction des graphes locaux
    vector<Marking> list_marquages;
    for (int index_module = 0; index_module < getModulesCount(); index_module++) {

        for (int i = 0; i < m_graphe_sync->getNodesCount(); i++) {
            NodeSG *node = m_graphe_sync->getNode(i);
            Marking marquage = *node->getMarquage(index_module);
            if (!marquage.isVide()
                && (list_marquages.end()
                    == find(list_marquages.begin(),
                            list_marquages.end(), marquage))) {
                list_marquages.push_back(marquage);
            }

        }
        m_modules[index_module]->getLocalSpaceReduced(list_marquages,
                                                      m_graphe_sync, m_espace->getEspaceLocal(index_module));
        list_marquages.clear();

    }
    return m_espace;

}

DistributedState *ModularPetriNet::buildReducedDSS() {
    m_dss = new DistributedState(this);
    m_dss->setNombreModules(getModulesCount());

    MetaState *ms;
    // Build initial meta-states
    cout << "Build initial meta-states" << endl;
    ProductSCC *productscc = new ProductSCC();
    vector<MetaState *> list_metatstates;
    for (int module = 0; module < getModulesCount(); module++) {
        ms = new MetaState();
        StateGraph *state_graph = m_modules[module]->getStateGraph(
                m_modules[module]->getMarquage());
        state_graph->setID(module);
        ms->setStateGraph(state_graph);
        m_dss->addMetaState(ms, module);
        //m_modules[module]->printMetaStateEx(ms);
        list_metatstates.push_back(ms);
        productscc->addSCC(ms->getInitialSCC());
    }

    // Setting name for meta-states
    for (int i = 0; i < getModulesCount(); i++) {
        list_metatstates.at(i)->setSCCProductName(productscc);
    }
    vector<RElement_dss> list_fusions;
    extractEnabledFusionReduced(list_metatstates, list_fusions);

    vector<RListProductFusion> stack_fusion;
    stack_fusion.push_back(list_fusions);

    while (!stack_fusion.empty()) {
        list_fusions = stack_fusion.back();
        stack_fusion.pop_back();
        for (int i = 0; i < list_fusions.size(); i++) {
            RElement_dss elt = list_fusions.at(i);
            for (int index_global_state = 0;
                 index_global_state < elt.m_gs->size();
                 index_global_state++) {
                vector<Marking *> *global_state = elt.m_gs->at(
                        index_global_state);
                for (int module = 0; module < getModulesCount(); module++) {
                    m_modules[module]->setMarquage(global_state->at(module));
                }
                Fusion *fusion = elt.m_fusion;
                fusion->tirer();

                // Build obtained destination Meta-states and check whether they exist or not
                MetaState *dest_ms;
                // Build initial meta-states
                //cout << "Build destination meta-states" << endl;
                ProductSCC *dest_productscc = new ProductSCC();
                vector<MetaState *> dest_list_metatstates;
                dest_list_metatstates.resize(getModulesCount());
                for (int module = 0; module < getModulesCount(); module++) {
                    if (fusion->participate(module)) {
                        dest_ms = new MetaState();

                        StateGraph *state_graph =
                                m_modules[module]->getStateGraph(
                                        m_modules[module]->getMarquage());
                        state_graph->setID(module);

                        dest_ms->setStateGraph(state_graph);

                        dest_list_metatstates[module] = dest_ms;
                        dest_productscc->addSCC(dest_ms->getInitialSCC());
                    } else
                        dest_productscc->addSCC(
                                elt.getMetaState(module)->getSCCProductName()->getSCC(
                                        module));

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
                for (int module = 0; module < getModulesCount(); module++) {
                    if (fusion->participate(module)) {
                        if (!m_dss->getMetaGraph(module)->findMetaStateByProductSCC(
                                dest_productscc)) {
                            ArcSync *arc_sync = new ArcSync();
                            MetaState *source_ms = elt.getMetaState(module);


                            arc_sync->setData(startProduct, fusion,
                                              dest_list_metatstates.at(module));
                            source_ms->addSyncArc(arc_sync);
                            //dest_list_metatstates.at(module)->addSyncArc(arc_sync);
                            m_dss->addMetaState(
                                    dest_list_metatstates.at(module), module);
                            //m_modules[module]->printMetaStateEx(
                            //		dest_list_metatstates.at(module));
                            dest_list_metatstates.at(module)->setSCCProductName(
                                    dest_productscc);
                            list_dest_metatstates.push_back(
                                    dest_list_metatstates.at(module));
                        } else {
                            exist = true;
                            ArcSync *arc_sync = new ArcSync();
                            MetaState *source_ms = elt.getMetaState(module);

                            MetaState *ms_dest =
                                    m_dss->getMetaGraph(module)->findMetaStateByProductSCC(
                                            dest_productscc);
                            arc_sync->setData(startProduct, fusion, ms_dest);
                            source_ms->addSyncArc(arc_sync);
                            list_dest_metatstates.push_back(ms_dest);

                        }

                    } else
                        list_dest_metatstates.push_back(
                                elt.getMetaState(module));

                }

                if (!exist) {
                    vector<RElement_dss> new_list_fusions;
                    extractEnabledFusionReduced(list_dest_metatstates,
                                                new_list_fusions);
                    stack_fusion.push_back(new_list_fusions);
                }

            }

        }

    }

    return m_dss;
}

DistributedState *ModularPetriNet::buildDSS() {
    m_dss = new DistributedState(this);
    m_dss->setNombreModules(getModulesCount());

    MetaState *ms;
    // Build initial meta-states
    //cout << "Build initial meta-states" << endl;
    ProductSCC *productscc = new ProductSCC();
    vector<MetaState *> list_metatstates;
    for (int module = 0; module < getModulesCount(); module++) {
        ms = new MetaState();
        StateGraph *state_graph = m_modules[module]->getStateGraph(
                m_modules[module]->getMarquage());
        state_graph->setID(module);
        //cout<<"I'm here"<<endl;
        ms->setStateGraph(state_graph);
        m_dss->addMetaState(ms, module);
        m_modules[module]->printMetaStateEx(ms);
        list_metatstates.push_back(ms);
        productscc->addSCC(ms->getInitialSCC());
    }

    // Setting name for meta-states
    for (int i = 0; i < getModulesCount(); i++) {
        list_metatstates.at(i)->setSCCProductName(productscc);
    }
    ListProductFusion list_fusions;
    extractEnabledFusion(productscc, list_fusions);

    vector<ListProductFusion> stack_fusion;
    stack_fusion.push_back(list_fusions);

    while (!stack_fusion.empty()) {
        list_fusions = stack_fusion.back();
        stack_fusion.pop_back();
        for (int i = 0; i < list_fusions.size(); i++) {
            Element_dss elt = list_fusions.at(i);
            for (int index_global_state = 0;
                 index_global_state < elt.m_gs->size();
                 index_global_state++) {
                vector<Marking *> *global_state = elt.m_gs->at(
                        index_global_state);
                for (int module = 0; module < getModulesCount(); module++) {
                    m_modules[module]->setMarquage(global_state->at(module));
                }
                Fusion *fusion = elt.m_fusion;
                fusion->tirer();

                // Build obtained destination Meta-states and check whether they exist or not
                MetaState *dest_ms;
                // Build initial meta-states
                //cout << "Build destination meta-states" << endl;
                ProductSCC *dest_productscc = new ProductSCC();
                vector<MetaState *> dest_list_metatstates;
                for (int module = 0; module < getModulesCount(); module++) {
                    dest_ms = new MetaState();

                    StateGraph *state_graph = m_modules[module]->getStateGraph(
                            m_modules[module]->getMarquage());
                    state_graph->setID(module);

                    dest_ms->setStateGraph(state_graph);
                    // m_dss->addMetaState(ms,module);
                    // m_modules[module]->printMetaStateEx(ms);
                    dest_list_metatstates.push_back(dest_ms);
                    dest_productscc->addSCC(dest_ms->getInitialSCC());
                }
                // cout << "Product built : " << getProductSCCName(dest_productscc)<<endl;

                // Check whether the computed product SCC exists or not
                if (!m_dss->getMetaGraph(0)->findMetaStateByProductSCC(
                        dest_productscc)) {
                    // Insert metastate into DSS
                    for (int module = 0; module < getModulesCount(); module++) {
                        ArcSync *arc_sync = new ArcSync();
                        MetaState *source_ms =
                                m_dss->getMetaGraph(module)->findMetaStateByProductSCC(
                                        elt.m_product);
                        ProductSCC startProduct;
                        for (int i = 0; i < global_state->size(); i++) {
                            MetaState *_source_ms =
                                    m_dss->getMetaGraph(i)->findMetaStateByProductSCC(
                                            elt.m_product);
                            SCC *_scc = _source_ms->findSCC(
                                    global_state->at(i));
                            startProduct.addSCC(_scc);
                        }
                        //cout<<"Produi ajouté "<<getProductSCCName(&startProduct)<<endl;
                        arc_sync->setData(startProduct, fusion,
                                          dest_list_metatstates.at(module));
                        source_ms->addSyncArc(arc_sync);
                        //dest_list_metatstates.at(module)->addSyncArc(arc_sync);
                        m_dss->addMetaState(dest_list_metatstates.at(module),
                                            module);
                        // m_modules[module]->printMetaStateEx(dest_list_metatstates.at(module));

                    }

                    // Setting name for meta-states
                    for (int i = 0; i < getModulesCount(); i++) {
                        dest_list_metatstates.at(i)->setSCCProductName(
                                dest_productscc);
                    }
                    ListProductFusion new_list_fusions;
                    extractEnabledFusion(dest_productscc, new_list_fusions);
                    stack_fusion.push_back(new_list_fusions);
                } else {
                    for (int module = 0; module < getModulesCount(); module++) {
                        ArcSync *arc_sync = new ArcSync();
                        MetaState *source_ms =
                                m_dss->getMetaGraph(module)->findMetaStateByProductSCC(
                                        elt.m_product);

                        ProductSCC startProduct;
                        for (int i = 0; i < global_state->size(); i++) {
                            MetaState *_source_ms =
                                    m_dss->getMetaGraph(i)->findMetaStateByProductSCC(
                                            elt.m_product);
                            SCC *_scc = _source_ms->findSCC(
                                    global_state->at(i));
                            startProduct.addSCC(_scc);
                        }
                        MetaState *ms_dest =
                                m_dss->getMetaGraph(module)->findMetaStateByProductSCC(
                                        dest_productscc);
                        arc_sync->setData(startProduct, fusion, ms_dest);
                        source_ms->addSyncArc(arc_sync);

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
        Fusion *fusion = m_fusions.at(index_fusion);
        bool canBeActive = true;
        for (int j = 0; j < getModulesCount() && canBeActive; j++) {
            if (fusion->participate(j) && fusion->participatePartially(j)) {
                canBeActive = false;
                ListLocalStates list_states = list_ms.at(j)->getListMarq(); //  m_dss->getLocalStates(product,j);
                for (int index = 0; index < list_states->size() && !canBeActive; index++) {
                    m_modules.at(j)->setMarquage(list_states->at(index));
                    if (fusion->isFranchissableLocal(j)) {
                        canBeActive = true;
                    }

                }
            }
        }

        if (canBeActive) {
            //////////////////////////////////////////////////////////////////////////
            // Déterminer tous les marquages locaux activant la fusion de transitions
            //////////////////////////////////////////////////////////////////////////

            states_enabling_fusion.resize(getModulesCount());
            for (int j = 0; j < getModulesCount(); j++) {
                ListLocalStates list_states = list_ms.at(j)->getListMarq();
                // states_enabling_fusion.at(j)= NULL;
                states_enabling_fusion.at(j) = new vector<Marking *>();
                if (fusion->participate(j)) {
                    for (int index = 0; index < list_states->size(); index++) {
                        m_modules.at(j)->setMarquage(list_states->at(index));
                        if (fusion->isFranchissableLocal(j)) {
                            states_enabling_fusion.at(j)->push_back(list_states->at(index));
                        }

                    }

                } else if (!fusion->participate(j)) {
                    states_enabling_fusion.at(j)->push_back(list_states->at(0));
                }

            }
            ListGlobalStates list_globalstates = computeSychronisedProduct(states_enabling_fusion);
            RElement_dss elt;
            elt.m_gs = list_globalstates;
            elt.m_fusion = fusion;
            elt.setListMetaStates(list_ms);
            list_elt.push_back(elt);
        }
    }

}
/*
 * @brief Determine enabled fusion transitions
 */

void ModularPetriNet::extractEnabledFusionV2(vector<MetaState *> &list_ms,vector<RElement_dss> &list_elt) {
    vector<ListLocalStates> states_enabling_fusion;
    for (const auto & fusion : m_fusions) {
        /************************************************/
        /**Check whether a fusion set is enabled or not**/
        /************************************************/
        bool canBeActive = true;
        for (int j = 0; j < getModulesCount() && canBeActive; j++) {
            if (fusion->participate(j) && fusion->participatePartially(j)) {
                canBeActive = false;
                ListLocalStates list_states = list_ms.at(j)->getListMarq(); //  m_dss->getLocalStates(product,j);
                for (int index = 0; index < list_states->size() && !canBeActive; index++) {
                    m_modules.at(j)->setMarquage(list_states->at(index));
                    if (fusion->isFranchissableLocal(j)) {
                        canBeActive = true;
                    }

                }
            }
        }

        if (canBeActive) {
            //////////////////////////////////////////////////////////////////////////
            // Déterminer tous les marquages locaux activant la fusion de transitions
            //////////////////////////////////////////////////////////////////////////
            states_enabling_fusion.resize(getModulesCount());
            for (int j = 0; j < getModulesCount(); j++) {
                ListLocalStates list_states = list_ms.at(j)->getListMarq();
                // states_enabling_fusion.at(j)= NULL;
                states_enabling_fusion.at(j) = new vector<Marking *>();
                if (fusion->participate(j)) {
                    for (int index = 0; index < list_states->size(); index++) {
                        m_modules.at(j)->setMarquage(list_states->at(index));
                        if (fusion->isFranchissableLocal(j)) {
                            states_enabling_fusion.at(j)->push_back(list_states->at(index));
                        }

                    }

                } else if (!fusion->participate(j)) {
                    states_enabling_fusion.at(j)->push_back(list_states->at(0));
                }

            }
            ListGlobalStates list_globalstates = computeSychronisedProduct(states_enabling_fusion);
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

    for (int index_fusion = 0; index_fusion < m_fusions.size(); index_fusion++) {
        /************************************************/
        /**Check whether a fusion set is enabled or not**/
        /************************************************/
        Fusion *fusion = m_fusions.at(index_fusion);
        bool canBeActive = true;
        for (int j = 0; j < getModulesCount() && canBeActive; j++) {
            if (fusion->participate(j) && fusion->participatePartially(j)) {
                canBeActive = false;
                ListLocalStates list_states = m_dss->getLocalStates(product, j);
                for (int index = 0; index < list_states->size() && !canBeActive; index++) {
                    m_modules.at(j)->setMarquage(list_states->at(index));
                    if (fusion->isFranchissableLocal(j)) {
                        canBeActive = true;
                    }

                }
            }
        }

        if (canBeActive) {
            //////////////////////////////////////////////////////////////////////////
            // D�terminer tous les marquages locaux activant la fusion de transitions
            //////////////////////////////////////////////////////////////////////////

            states_enabling_fusion.resize(getModulesCount());
            for (int j = 0; j < getModulesCount(); j++) {
                ListLocalStates list_states = m_dss->getLocalStates(product, j);
                // states_enabling_fusion.at(j)= NULL;
                states_enabling_fusion.at(j) = new vector<Marking *>();
                if (fusion->participate(j)) {
                    for (int index = 0; index < list_states->size(); index++) {
                        m_modules.at(j)->setMarquage(list_states->at(index));
                        if (fusion->isFranchissableLocal(j)) {
                            states_enabling_fusion.at(j)->push_back(list_states->at(index));
                        }

                    }

                } else if (!fusion->participate(j)) {

                    states_enabling_fusion.at(j)->push_back(list_states->at(0));

                }

            }

            ListGlobalStates list_globalstates = computeSychronisedProduct(states_enabling_fusion);
            Element_dss elt;
            elt.m_gs = list_globalstates;
            elt.m_fusion = fusion;
            elt.m_product = product;
            list_elt.push_back(elt);
            /*for (unsigned int i=0;i<liste_node->size();i++)
             {
             node->addArc(liste_node->at(i),fusion,NULL);
             }*/

        }
    }
}

ListGlobalStates ModularPetriNet::computeSychronisedProduct(vector<ListLocalStates> &states_enabling_fusion) {
    vector<int> indices;
    indices.resize(getModulesCount());
    fill(indices.begin(), indices.end(), 0);

    ListGlobalStates myliste = new vector<ListLocalStates>();
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

        vector<Marking *> *marq = new vector<Marking *>();
        for (int i = 0; i < states_enabling_fusion.size(); i++) {
            marq->push_back(states_enabling_fusion[i]->at(indices[i]));
        }
        myliste->push_back(marq);
        //V�rifier si on a aprcouru toutes les combinaisons
        result = true;
        for (int i = 0; i < states_enabling_fusion.size() && result; i++) {
            if ((states_enabling_fusion[i]->size() - 1) != indices[i]) result = false;
        }
    } while (result == false);
    return myliste;

}

void ModularPetriNet::writeToFile(const string filename) {
    std::ofstream myfile;
    myfile.open(filename);
    myfile << "digraph " << "fichier " << "{" << endl;
    myfile << "compound=true" << endl;
    for (int module = 0; module < getModulesCount(); module++) {
        PetriNet *petri = m_modules.at(module);

        for (int i = 0; i < m_dss->getMetaGraph(module)->getMetaStateCount();
             i++) {

            MetaState *ms = m_dss->getMetaGraph(module)->getMetaState(i);
            ProductSCC *pscc = ms->getSCCProductName();

            myfile << "subgraph cluster" << getProductSCCName(pscc) << module
                   << " {" << endl;
            /*for (int jj = 0; jj < ms->getListArcs()->size(); jj++) {
                Marking *source_marq = ms->getListArcs()->at(jj).getSource();
                Marking *dest_marq =
                        ms->getListArcs()->at(jj).getDestination();
                myfile << petri->getMarquageName(*source_marq)
                        << getProductSCCName(pscc) << module << " -> ";
                myfile << petri->getMarquageName(*dest_marq)
                        << getProductSCCName(pscc) << module;
                myfile << " [label=\""
                        << ms->getListArcs()->at(jj).getTransition()->getName()
                        << "\"]" << endl;
            }*/
            myfile << "label=\"" << getProductSCCName(pscc) << "\"" << endl;
            myfile << "}" << endl;

            for (int k = 0; k < ms->getSucc().size(); k++) {
                myfile << petri->getSCCName(pscc->getSCC(module))
                       << getProductSCCName(pscc) << module << " -> ";
                ArcSync *arc = ms->getSucc().at(k);
                MetaState *ms_dest = arc->getMetaStateDest();
                myfile
                        << petri->getSCCName(
                                ms_dest->getSCCProductName()->getSCC(module))
                        << getProductSCCName(ms_dest->getSCCProductName())
                        << module;
                myfile << " [ltail=cluster" << getProductSCCName(pscc) << module
                       << ",lhead=cluster"
                       << getProductSCCName(ms_dest->getSCCProductName())
                       << module << "]" << endl;
                //myfile<<arc->getFusion()->getName()<<"]";
                //myfile<<endl;

            }
        }
    }
    myfile << "}" << endl;
    myfile.close();
}

string ModularPetriNet::getProductSCCName(ProductSCC *pss) {
    string res = "";
    for (int module = 0; module < getModulesCount(); module++) {
        PetriNet *petri = m_modules.at(module);
        res += petri->getSCCName(pss->getSCC(module));
    }
    return res;
}

void ModularPetriNet::writeTextFile(const string filename) {
    std::ofstream myfile;
    myfile.open(filename);

    for (int module = 0; module < getModulesCount(); module++) {
        myfile << "******** Module " << module << " ********" << endl;
        myfile << "#Meta-states : "
               << m_dss->getMetaGraph(module)->getMetaStateCount() << endl;
        PetriNet *petri = m_modules.at(module);

        for (int i = 0; i < m_dss->getMetaGraph(module)->getMetaStateCount(); i++) {

            MetaState *ms = m_dss->getMetaGraph(module)->getMetaState(i);
            ProductSCC *pscc = ms->getSCCProductName();
            myfile << "Metastate : " << getProductSCCName(pscc) << endl;
            for (auto source_mark: *ms->getListMarq()) {
                myfile << petri->getMarquageName(*source_mark);
                for (auto succ: *source_mark->getListSucc()) {
                    Transition *t = succ.first;
                    Marking *dest_marq = succ.second;

                    myfile << " - " << t->getName() << " -> ";
                    myfile << petri->getMarquageName(*dest_marq) << endl;
                }
            }
            myfile << endl;
        }
        // Compute #(sync arcs)
        unsigned int total_arcs = 0;
        for (int i = 0; i < m_dss->getMetaGraph(module)->getMetaStateCount(); i++) {
            MetaState *ms = m_dss->getMetaGraph(module)->getMetaState(i);
            total_arcs += ms->getSucc().size();
        }
        myfile << "#Synchronisation arcs : " << total_arcs << endl;
        // List sync arcs
        for (int i = 0; i < m_dss->getMetaGraph(module)->getMetaStateCount(); i++) {

            MetaState *ms = m_dss->getMetaGraph(module)->getMetaState(i);
            ProductSCC *pscc = ms->getSCCProductName();
            for (int k = 0; k < ms->getSucc().size(); k++) {
                myfile << getProductSCCName(pscc) << " ==== ";
                ArcSync *arc = ms->getSucc().at(k);
                MetaState *ms_dest = arc->getMetaStateDest();
                myfile << "(" << getProductSCCName(arc->getStartProduct())
                       << "," << arc->getFusion()->getName() << ") ===> ";
                myfile << getProductSCCName(ms_dest->getSCCProductName()) << endl;
                //myfile<<arc->getFusion()->getName()<<"]";
                //myfile<<endl;

            }
        }
    }

    myfile.close();
}
