// PetriNet.h: interface for the PetriNet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PETRINET_H__91DDDBD6_8669_4EC2_A0F5_E20F2FAFDC37__INCLUDED_)
#define AFX_PETRINET_H__91DDDBD6_8669_4EC2_A0F5_E20F2FAFDC37__INCLUDED_

#include <memory>
#include <set>
#include <unordered_map>
#include "DSS/Marking.h"
#include "DSS/LocalGraph.h"
#include "Petri/Place.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Transition;


class MetaState;

class SCC;

using std::vector;

class PetriNet {
public:
    string getSCCName(SCC *scc);

    int addInputPlaces(string transition_name, vector<string> input_places, vector<int> weights);

    Transition *getTransitionAddress(const string transition_name);

    int getNumber();

    void setNumber(const int index);

    string getMarkingName(const Marking &marking);

    int addOutputPlaces(string transition_name, vector<string> output_places, vector<int> weights);

    void fire(Transition &t);

    void setMarking(const Marking *marking);

    Marking getMarking();

    Place getPlace(const int index);

    Transition *getTransitionAddress(const int code);

    void addListTransitions(vector<Transition> transitions);

    void addListPlaces(vector<Place> places);

    int getPlaceCount();

    PetriNet()=default;

    virtual ~PetriNet();

    // A new metastate whose markings are those reached from marking by local transitions. Its local graph is shared
    // with the metastates having the same markings (see getLocalGraph())
    MetaState *getMetaState(const Marking &marking);

    // Local graph of the markings reached from marking by local transitions. Graphs are stored once: if marking is in
    // the initial SCC of a graph already computed, this graph is returned without exploring (all its markings are
    // reached from marking); otherwise, after the exploration, a graph equal to an existing one is replaced by it
    LocalGraph *getLocalGraph(const Marking &marking);

    // SCC of marking in a local graph already computed, nullptr if no graph contains it. The SCC of a marking only
    // depends on the marking (a graph is closed under the local transitions)
    [[nodiscard]] SCC *findComputedSCC(const Marking &marking) const;

    // Number of local graphs stored and of markings in them
    [[nodiscard]] size_t getLocalGraphCount() const { return m_graphs.size(); }
    [[nodiscard]] size_t getStoredMarkingCount() const {
        size_t count {};
        for (const auto &graph: m_graphs) count += graph->getListMarkings().size();
        return count;
    }

    void printMetaStateEx(MetaState *ms);

    bool areTransitionsIncluded(const std::set<string> &list_transitions);
private:
    int m_number;

    // A local (non synchronised) transition, with the effect of its firing on each place
    struct LocalTransition {
        Transition *transition;
        vector<std::pair<uint32_t, uint32_t>> inputs; // (place index, weight): needed tokens
        vector<std::pair<uint32_t, int>> deltas;      // (place index, output - input) for the places it changes
    };
    // Local transitions, in the order of m_transitions; built at the first call of getMetaState()
    vector<LocalTransition> m_local_transitions;
    bool m_local_transitions_built {false};
    void buildLocalTransitions();
    // Explore the markings reached from marking: a new local graph, with its SCCs
    std::unique_ptr<LocalGraph> exploreLocalGraph(const Marking &marking);

    // The local graphs computed, owned until the end (the names of the metastates refer to their SCCs)
    vector<std::unique_ptr<LocalGraph>> m_graphs;
    // Local graphs indexed by their content hash, to find an equal one
    std::unordered_multimap<size_t, LocalGraph *> m_graph_index;
    // SCC of each marking of the local graphs (in the first graph containing it)
    std::unordered_map<const Marking *, SCC *, Marking::PtrHash, Marking::PtrEqual> m_scc_index;

    Place *getPlaceAddress(string placename);

    vector<Place> m_places;
    vector<Transition> m_transitions;

};

#endif // !defined(AFX_PETRINET_H__91DDDBD6_8669_4EC2_A0F5_E20F2FAFDC37__INCLUDED_)
