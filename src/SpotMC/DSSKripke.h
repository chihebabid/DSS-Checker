//
// Created by chiheb on 04/10/23.
//

#ifndef DISTRIBUTEDSTATESPACE_DSSKRIPKE_H
#define DISTRIBUTEDSTATESPACE_DSSKRIPKE_H
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <spot/twa/twa.hh>
#include "DSS/ModuleSS.h"
#include "SpotMC/DSSIterator.h"
#include "SpotMC/Propositions.h"

/*
 * Automaton of the state space of one module, to be intersected with the automaton of a formula.
 * States are markings of metastates (DSSStateKey): the run goes from a marking to another one, by local transitions
 * or by synchronisations (sync arcs of the metastates). The atomic propositions of the formula are
 * transition names (action-based), carried by the edges, and conditions on the marking of the module
 * (state-based): the edges leaving a state carry the values of these conditions in its marking (the word read
 * is the sequence of the states of the run, as in a Kripke structure).
 *
 * The run of a module that stops moving forever ends in a divergence state looping without firing a transition.
 * With state-based propositions, there is one divergence state per marking: it keeps the marking where the
 * module stopped (with transition names only, a single divergence state is shared).
 *
 * It derives from spot::twa and not from spot::kripke: Spot assumes that all the edges leaving a
 * state of a Kripke structure have the same label (state-based labels), which is not the case here.
 */
class DSSKripke : public spot::twa {
public:
    // propositions: atomic propositions of the formula, resolved on the module (transitions and state predicates).
    // fairness: executions under fairness of the synchronisations (see getSuccessors()); otherwise every
    // execution is considered, as in the symbolic observation graphs (LTL\X without fairness)
    DSSKripke(const spot::bdd_dict_ptr &dict_ptr, ModuleSS *metagraph, const ResolvedPropositions &propositions,
              bool fairness = false);

    ~DSSKripke() override;

    const spot::state *get_init_state() const override;

    DSSIterator *succ_iter(const spot::state *s) const override;

    std::string format_state(const spot::state *s) const override;

    // Label of an edge firing transition t (nullptr: no transition of the formula is fired):
    // the proposition of t is true and the other ones are false
    [[nodiscard]] bdd edgeCondition(const Transition *t) const;

    // Values of the state-based propositions in marking (bddtrue if the formula has none)
    [[nodiscard]] bdd stateLabel(const Marking *marking) const;

    // True if s is a divergence state
    [[nodiscard]] static bool isDivergence(const DSSStateKey &s) { return s.metastate == nullptr; }

    // The formula has state-based / transition propositions (fast paths of DSSIterator::cond())
    [[nodiscard]] bool hasPredicates() const { return !m_predicates.empty(); }
    [[nodiscard]] bool hasTransitionAps() const { return !m_ap_vars.empty(); }

    [[nodiscard]] size_t getIdModule() const { return m_metagraph->getIdModule(); }

    // Successor of a state: (destination, fired transition of the module or nullptr)
    using Successors = std::vector<std::pair<DSSStateKey, Transition *>>;

    // Successors of state, as seen by Spot (computed once, then cached).
    // Synchronisations in which the module does not take part are invisible: the module keeps its marking
    // but moves to the destination metastate. They are not edges: they are followed by a closure, and the
    // successors of state are those of every state of its closure (see DSSKripke.cpp)
    const Successors &getSuccessors(const DSSStateKey &state) const;

private:
    ModuleSS *m_metagraph;
    bool m_fairness;
    std::map<std::string, bdd> m_ap_vars; // BDD variable of each transition proposition
    bdd m_no_ap {bddtrue};                // All the transition propositions are false
    std::vector<std::pair<StatePredicate, bdd>> m_predicates; // State-based propositions and their BDD variable
    mutable Marking m_divergence {0};     // Marking of the shared divergence state (transition propositions only)
    mutable std::unordered_map<DSSStateKey, Successors, DSSStateKey::Hash> m_successors; // Cache of getSuccessors()
};


#endif //DISTRIBUTEDSTATESPACE_DSSKRIPKE_H
