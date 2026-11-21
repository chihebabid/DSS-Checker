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

    // Size of the DSS, per module and in total (computed on the built, possibly reduced, DSS)
    struct Stats {
        struct Row {
            size_t metastates {};    // meta-states of the module
            size_t sccs {};          // SCCs, summed over the meta-states
            size_t sync_arcs {};     // synchronisation arcs leaving the meta-states
            size_t local_arcs {};    // local arcs, summed over the meta-states
            size_t states {};        // markings, summed over the meta-states (a marking shared by k counts k times)
            size_t stored_states {}; // markings actually stored (once per distinct local graph)
            size_t local_graphs {};  // distinct local graphs stored
        };
        vector<Row> modules;
        Row total;
        size_t built_metastates {}; // meta-states built, before the reduction merges some of them
    };

    Stats computeStats();

    // Print the statistics of the DSS as a table, one row per module plus a total row
    void printStats(std::ostream &out);

    // Save the statistics of the DSS in a text file
    void outputTXT(const string &filename);

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
    size_t m_built_metastates {}; // see Stats::built_metastates
    DistributedSS m_dss;
    // Label of the arcs of the modules that do not participate in their fusion (see arcLabel()); it has no transition
    TransitionFusionSet m_tau;
};


#endif //DISTRIBUTEDSTATESPACE_DSSBUILDER_H
