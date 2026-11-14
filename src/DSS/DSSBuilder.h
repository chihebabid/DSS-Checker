//
// Created by chiheb on 13/07/22.
//

#ifndef DISTRIBUTEDSTATESPACE_DSSBUILDER_H
#define DISTRIBUTEDSTATESPACE_DSSBUILDER_H

#include "Petri/ModularPetriNet.h"
#include "DSS/ModuleSS.h"
#include "DSS/DistributedSS.h"
#include <iosfwd>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class DSSBuilder {
public:
    // reduction: reduce the metastates by bisimulation once the DSS is built (see ModuleSS::reduceByBisimulation())
    explicit DSSBuilder(ModularPetriNet *ptr, bool reduction = false);

    void build();

    // Write the graph in a dot file
    void writeToFile(const string &filename);

    // Render the graph in an image file (format given by the extension: svg, png, pdf, ...).
    // Requires Graphviz (dot and neato). Reflexive sync arcs are drawn as loops below their metastate.
    bool renderToFile(const string &filename);

    void outputTXT();

    ModuleSS* getModuleGraph(const size_t index) const;

private:
    // A reflexive sync arc: the dot cluster of its metastate and its label
    struct SyncLoop {
        string cluster;
        string label;
    };

    // Write the graph in dot format. If loops is given (layout mode), the reflexive sync arcs
    // are hidden and stored in *loops
    void writeDot(std::ostream &stream, vector<SyncLoop> *loops);

    string getProductSCCName(const ProductSCC *pss);

    // Name, in the dot file, of the cluster of scc in the metastate named pss of module
    string getSCCClusterName(SCC *scc, const ProductSCC *pss, int module);

    // Name of the source product of arc: the SCC of every module not synchronised on arc is displayed as "-"
    string getSourceProductName(const ArcSync *arc);


    void buildInitialMS();

    // Label of an arc of module for fusion: fusion if module participates in it, otherwise tau (the source product
    // is kept). Seen from a module that does not participate, the fusions are indistinguishable: with the same
    // label, the reduction by bisimulation can merge more metastates
    TransitionFusionSet *arcLabel(TransitionFusionSet *fusion, int module);

    // Free the metastates merged by the reduction (see the .cpp)
    void freeUnusedMetaStates(const vector<vector<MetaState *>> &built);

    ModularPetriNet *m_petri;
    bool m_reduction;
    DistributedSS m_dss;
    // Label of the arcs of the modules that do not participate in their fusion (see arcLabel()); it has no transition
    TransitionFusionSet m_tau;
};


#endif //DISTRIBUTEDSTATESPACE_DSSBUILDER_H
