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
#include <vector>

class DSSBuilder {
public:
    DSSBuilder(ModularPetriNet *ptr);

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

    MetaState *reduce(MetaState *ms, const int &module);

    void buildInitialMS();

    ModularPetriNet *m_petri;
    DistributedSS m_dss;
};


#endif //DISTRIBUTEDSTATESPACE_DSSBUILDER_H
