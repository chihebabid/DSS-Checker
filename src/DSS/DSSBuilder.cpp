//
// Created by chiheb on 13/07/22.
//

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <random>
#include <sstream>
#include <memory>
#include "DSS/DSSBuilder.h"
#include "DSS/ArcSync.h"
#include <fstream>
#include <algorithm>

// Enable/disable reduction on the fly
//#define REDUCTION

DSSBuilder::DSSBuilder(ModularPetriNet *ptr, const bool reduction) : m_petri(ptr), m_reduction(reduction) {
    for (size_t i{}; i < ModularPetriNet::getModuleCount(); ++i) {
        m_dss.addModule(new ModuleSS(i));
    }
    m_reference_ms.resize(ModularPetriNet::getModuleCount());
}

MetaState *DSSBuilder::getReferenceMetaState(const int module, const Marking &marking) {
    auto &references {m_reference_ms[module]};
    if (const auto it {references.find(const_cast<Marking *>(&marking))}; it != references.end()) return it->second;
    MetaState *reference {m_petri->getModule(module)->getMetaState(marking)};
    references.emplace(reference->getListMarkings()[0], reference);
    return reference;
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
    // Initial state of each module (its initial metastate may be merged into another one by the reduction)
    for (size_t module {}; module < list_metastates.size(); ++module) {
        m_dss[module]->setInitialState(productscc, *list_metastates[module]->getListMarkings()[0]);
    }

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

                // Name of the destination metastates: product of the initial SCCs of the reached markings.
                // The destination metastates themselves are only computed below, when they are new
                //cout << "Build destination meta-states" << endl;
                auto *dest_productscc = new ProductSCC();
                vector<MetaState *> dest_references(ModularPetriNet::getModuleCount(), nullptr);
                for (int module = 0; module < ModularPetriNet::getModuleCount(); ++module) {
#ifdef REDUCTION
                    if (fusion->participate(module)) {
#endif

                    dest_references[module] = getReferenceMetaState(module, m_petri->getModule(module)->getMarking());
                    dest_productscc->addSCC(dest_references[module]->getInitialSCC());
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
                        // Module i is not synchronised on fusion: any SCC of its metastate
                        startProduct.addSCC(dest_productscc->getSCC(i), true);
                    }
                }
                // Check whether the computed product SCC exists or not
                vector<MetaState *> list_dest_metastates;
                bool exist = false;
                bool named = false; // dest_productscc has been given to a metastate as its name
                for (int module{}; module < ModularPetriNet::getModuleCount(); ++module) {
#ifdef REDUCTION
                    if (fusion->participate(module)) {
#endif
                    if (!m_dss[module]->findMetaStateByProductSCC(*dest_productscc)) {
                        // New destination metastate: computed from the reached marking
                        MetaState *dest_ms {m_petri->getModule(module)->getMetaState(
                            *dest_references[module]->getListMarkings()[0])};
                        auto arc_sync{std::make_unique<ArcSync>()};
                        MetaState *source_ms = elt.getMetaState(module);
                        arc_sync->setData(startProduct, fusion, dest_ms);
                        source_ms->addSyncArc(std::move(arc_sync));
                        m_dss[module]->insertMS(dest_ms);
                        dest_ms->setProductSCCName(dest_productscc);
                        named = true;
                        list_dest_metastates.push_back(dest_ms);
                    } else {
                        exist = true;
                        auto arc_sync{std::make_unique<ArcSync>()};
                        MetaState *source_ms = elt.getMetaState(module);
                        MetaState *ms_dest = m_dss[module]->findMetaStateByProductSCC(*dest_productscc);
                        arc_sync->setData(startProduct, fusion, ms_dest);
                        source_ms->addSyncArc(std::move(arc_sync));
                        list_dest_metastates.push_back(ms_dest);
                    }

#ifdef REDUCTION
                    } else list_dest_metastates.push_back(elt.getMetaState(module));
#endif

                }

                // Otherwise, the name only served to look the destination metastates up
                if (!named) delete dest_productscc;

                if (!exist) {
                    vector<RElement_dss> new_list_fusions;
                    m_petri->extractEnabledFusionReduced(list_dest_metastates, new_list_fusions);
                    stack_fusion.push_back(new_list_fusions);
                }
            }
        }
    }
    // Reduction of the metastates by bisimulation
    if (m_reduction) {
        size_t before {}, removed {};
        for (size_t module {}; module < ModularPetriNet::getModuleCount(); ++module) {
            before += m_dss[module]->getMetaStateCount();
            removed += m_dss[module]->reduceByBisimulation();
        }
        cout << "Reduction by bisimulation: " << before << " -> " << before - removed << " metastates" << endl;
    }
    // Lazy version for computing Sync-closure property
    for (size_t module{};module<ModularPetriNet::getModuleCount();++module) {
        for (auto ms: m_dss[module]->getListMetaStates()) {
            m_dss[module]->computeSyncClosedSCC(ms);
        }
    }
    // SCCs where a module may stay forever because another module can loop forever
    for (size_t module{};module<ModularPetriNet::getModuleCount();++module) {
        for (auto ms: m_dss[module]->getListMetaStates()) {
            m_dss[module]->markOthersCanLoop(ms);
        }
    }
}


void DSSBuilder::writeToFile(const string &filename) {
    std::ofstream myfile;
    myfile.open(filename);
    writeDot(myfile, nullptr);
    myfile.close();
}

void DSSBuilder::writeDot(std::ostream &myfile, vector<SyncLoop> *loops) {
    // In layout mode, the helper nodes of the reflexive sync arcs are invisible: they only reserve room
    const string hidden {loops ? ",style=invis" : ""};
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

            // One nested cluster per SCC, containing the markings of the SCC
            for (const auto &scc: *ms->getListSCCs()) {
                const auto sccName {petri->getSCCName(scc)};
                myfile << "subgraph " << getSCCClusterName(scc, pscc, module) << " {" << endl;
                for (const auto &marking: *scc->getListStates()) {
                    auto markingName = petri->getMarkingName(*marking);
                    myfile << markingName << getProductSCCName(pscc);
                    myfile << " [label=\"" << markingName << "\"] ";
                    myfile << ";\n";
                }
                myfile << "label=\"" << sccName << "\"" << endl;
                // A sync-closed SCC is drawn in bold green
                if (scc->isSyncClosed()) myfile << "style=\"dashed,bold\";color=green;fontcolor=green" << endl;
                else myfile << "style=dashed;color=black;fontcolor=black" << endl;
                myfile << "}" << endl;
            }

            for (const auto &source: ms->getListMarkings()) {
                if (!loops) {
                    std::cout<<petri->getMarkingName(*source)<<"is "<<source->getSCCContainer()->isTerminal()<<"\n";
                    std::cout<<petri->getMarkingName(*source)<<"closure is "<<source->getSCCContainer()->isSyncClosed()<<"\n";
                }
                auto sourceName = petri->getMarkingName(*source);
                const auto &lsucc = source->getListSucc();
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
            myfile << "style=solid" << endl;
            myfile << "}" << endl;

            // A sync arc links a metastate to another one (ltail/lhead are the metastate clusters)
            const string sourceCluster {"cluster" + getProductSCCName(pscc) + std::to_string(module)};
            // A sync arc is attached to a marking node of each metastate: the representative marking of the SCC of its name
            const auto anchorNode = [&](MetaState *metastate) {
                const auto name {metastate->getProductSCCName()};
                return petri->getMarkingName(*name->getSCC(module)->getListStates()->at(0)) + getProductSCCName(name);
            };
            const string sourceNode {anchorNode(ms)};
            size_t loop_count {};
            for (const auto &arc: ms->getSyncSucc()) { // Build output edges of Metastate ms
                MetaState *ms_dest = arc->getDestination();
                const string destCluster {"cluster" + getProductSCCName(ms_dest->getProductSCCName()) + std::to_string(module)};
                const string label {"(" + getSourceProductName(arc.get()) + "," + arc->getFusion()->getName() + ")"};
                if (destCluster == sourceCluster) {
                    // Graphviz cannot draw an edge from a cluster to itself: the loop goes through
                    // a helper node outside the metastate, which carries the label.
                    // renderToFile() hides it and draws the loop itself, with explicit coordinates.
                    const string loopNode {"loop_" + sourceCluster + "_" + std::to_string(loop_count++)};
                    myfile << loopNode << " [shape=plaintext,fontcolor=red,width=0,height=0,margin=0.05,label=\"" << label << "\"" << hidden << "]" << endl;
                    myfile << sourceNode << " -> " << loopNode << " [ltail=" << sourceCluster << ",color=red,arrowhead=none" << hidden << "]" << endl;
                    myfile << sourceNode << " -> " << loopNode << " [ltail=" << sourceCluster << ",color=red,dir=back" << hidden << "]" << endl;
                    if (loops) loops->push_back({sourceCluster, label});
                    continue;
                }
                myfile << sourceNode << " -> " << anchorNode(ms_dest);
                myfile << " [lhead=" << destCluster << ",ltail=" << sourceCluster;
                myfile << ",color=red,shape=curve,label=\"" << label << "\"]" << endl;
            }
        }
    }
    myfile << "}" << endl;
}

// Quote a file name for the shell
static string shellQuote(const string &text) {
    string res {"'"};
    for (const char c: text) {
        if (c == '\'') res += "'\\''";
        else res += c;
    }
    return res + "'";
}

/*
 * @brief Render the graph in an image file, in two Graphviz passes:
 *        1. dot computes the layout;
 *        2. each reflexive sync arc, that Graphviz cannot draw on a cluster, is added with explicit coordinates
 *           as a loop from the bottom-left to the bottom-right corner of its metastate, and neato -n2 renders.
 * @param filename image file; its extension gives the format (svg, png, pdf, ...)
 * @return false if Graphviz (dot and neato) is missing or fails
 */
bool DSSBuilder::renderToFile(const string &filename) {
    namespace fs = std::filesystem;
    const double loop_depth {28}, loop_step {26}, arrow {10}, font_size {14}; // in points

    string format {fs::path(filename).extension().string()};
    format = format.empty() ? "svg" : format.substr(1);

    const fs::path dir {fs::temp_directory_path() / ("dss_render_" + std::to_string(std::random_device{}()))};
    fs::create_directories(dir);
    struct Cleaner {
        fs::path dir;
        ~Cleaner() { std::error_code ec; fs::remove_all(dir, ec); }
    } cleaner {dir};
    const string pass1 {(dir / "pass1.dot").string()}, laid {(dir / "laid.dot").string()}, final_dot {(dir / "final.dot").string()};

    // Pass 1: layout
    vector<SyncLoop> loops;
    {
        std::ofstream file(pass1);
        writeDot(file, &loops);
    }
    if (std::system(("dot -Tdot -o " + shellQuote(laid) + " " + shellQuote(pass1)).c_str()) != 0) {
        std::cerr << "Error: Graphviz (dot) is missing or failed; " << filename << " not written\n";
        return false;
    }
    std::stringstream buffer;
    buffer << std::ifstream(laid).rdbuf();
    string graph {buffer.str()};

    // Add the loops, using the bounding boxes of the metastates computed by dot
    std::ostringstream extra;
    extra << std::fixed << std::setprecision(2);
    std::map<string, size_t> loop_counts;
    for (size_t index {}; index < loops.size(); ++index) {
        const auto &[cluster, label] {loops[index]};
        double left, bottom, right, top;
        const auto pos_cluster {graph.find("subgraph " + cluster + " {")};
        const auto pos_bb {pos_cluster == string::npos ? string::npos : graph.find("bb=\"", pos_cluster)};
        if (pos_bb == string::npos
            || std::sscanf(graph.c_str() + pos_bb, "bb=\"%lf,%lf,%lf,%lf", &left, &bottom, &right, &top) != 4) {
            std::cerr << "Error: metastate " << cluster << " not found in the layout; " << filename << " not written\n";
            return false;
        }
        const double depth {loop_depth + loop_counts[cluster]++ * loop_step};
        const double middle {(left + right) / 2}, label_y {bottom - depth - font_size * 0.8};
        const string name {"__dss_loop_" + std::to_string(index)};
        const string corner {" [shape=point,style=invis,width=0.01,height=0.01,label=\"\",pos=\""};
        extra << name << "_a" << corner << left << "," << bottom << "\"];\n";
        extra << name << "_b" << corner << right << "," << bottom << "\"];\n";
        // Invisible node below the label: it makes the drawing area include the whole loop
        extra << name << "_c" << corner << middle << "," << label_y - font_size << "\"];\n";
        // Bezier curve leaving the left corner downwards and coming back up to the right corner
        extra << name << "_a -> " << name << "_b [color=red,fontcolor=red,fontsize=" << font_size
              << ",label=\"" << label << "\",lp=\"" << middle << "," << label_y << "\",pos=\"e," << right << "," << bottom
              << " " << left << "," << bottom << " " << left << "," << bottom - 1.3 * depth
              << " " << right << "," << bottom - 1.3 * depth << " " << right << "," << bottom - arrow << "\"];\n";
    }
    const auto pos_end {graph.rfind('}')};
    if (pos_end == string::npos) {
        std::cerr << "Error: unexpected Graphviz output; " << filename << " not written\n";
        return false;
    }
    graph.insert(pos_end, extra.str());
    {
        std::ofstream file(final_dot);
        file << graph;
    }

    // Pass 2: render with the positions computed in pass 1
    if (std::system(("neato -n2 -T" + format + " -o " + shellQuote(filename) + " " + shellQuote(final_dot)).c_str()) != 0) {
        std::cerr << "Error: Graphviz (neato) is missing or failed; " << filename << " not written\n";
        return false;
    }
    return true;
}

string DSSBuilder::getSCCClusterName(SCC *scc, const ProductSCC *pss, const int module) {
    return "cluster_scc_" + m_petri->getModule(module)->getSCCName(scc) + getProductSCCName(pss) + std::to_string(module);
}

string DSSBuilder::getProductSCCName(const ProductSCC *pss) {
    string res;
    PetriNet *petri {};
    for (int module = 0; module < ModularPetriNet::getModuleCount(); ++module) {
        petri = m_petri->getModule(module);
        res += petri->getSCCName(pss->getSCC(module));
    }
    return res;
}

string DSSBuilder::getSourceProductName(const ArcSync *arc) {
    string res;
    for (int module = 0; module < ModularPetriNet::getModuleCount(); ++module) {
        const auto scc {arc->getSourceSCC(module)};
        res += scc ? m_petri->getModule(module)->getSCCName(scc) : "-";
    }
    return res;
}

/*
 * @brief Check whether a metastate can be fused with another
 * @param ms a Metastate
 * @param module Module index
 * @return true if *ms can be fusedm else false
 */
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