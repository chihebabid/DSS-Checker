//
// Created by chiheb on 04/10/23.
//

#include "SpotMC/DSSKripke.h"
#include "SpotMC/DSSState.h"
#include "Petri/Transition.h"
#include "Petri/ModularPetriNet.h"

DSSKripke::DSSKripke(const spot::bdd_dict_ptr &dict_ptr, ModuleSS *metagraph, const std::set<std::string> &aps,
                     const bool fairness)
        : spot::twa(dict_ptr), m_metagraph(metagraph), m_fairness(fairness) {
    set_generalized_buchi(0); // No acceptance set: every infinite run is accepting
    for (const auto &ap: aps) {
        const bdd var {bdd_ithvar(register_ap(ap))};
        m_ap_vars.emplace(ap, var);
        m_no_ap &= !var;
    }
}

DSSKripke::~DSSKripke() {
    get_dict()->unregister_all_my_variables(this);
}

const spot::state *DSSKripke::get_init_state() const {
    return new DSSState({m_metagraph->getInitialMetaState(), m_metagraph->getInitialMarking()});
}

DSSIterator *DSSKripke::succ_iter(const spot::state *s) const {
    return new DSSIterator(static_cast<const DSSState *>(s)->getKey(), this);
}

std::string DSSKripke::format_state(const spot::state *s) const {
    const auto marking {static_cast<const DSSState *>(s)->getKey().marking};
    if (marking == &m_divergence) return "divergence";
    const auto petri {ModularPetriNet::getInstance().getModule(static_cast<int>(getIdModule()))};
    return petri->getMarkingName(*marking) + " (SCC " + petri->getSCCName(marking->getSCCContainer()) + ")";
}

bdd DSSKripke::edgeCondition(const Transition *t) const {
    if (t == nullptr) return m_no_ap;
    const auto it {m_ap_vars.find(t->getName())};
    if (it == m_ap_vars.end()) return m_no_ap; // transition not observed by the formula
    // t is true, the other propositions are false
    return it->second & bdd_exist(m_no_ap, it->second);
}

/*
 * Successors of marking m:
 *  1. Closure of m: the markings reached from m by synchronisations in which the module does not take part
 *     (arcs whose source product has "-" for the module). The module does not move: each one is the marking
 *     equal to m in the destination metastate. These moves are invisible (erased from the word seen by the
 *     formula), so they are not edges labelled by "no transition".
 *  2. For every marking of the closure: its local transitions, and the synchronisations in which the module
 *     takes part and that can be fired from it.
 *  3. An edge to the divergence state if the module may stop moving forever.
 *     Without fairness (default): a marking of the closure is in
 *     a sync-closed SCC or in an SCC where another module can loop, or the closure contains a cycle
 *     (the other modules can synchronise forever without the module).
 *     Under fairness: only a marking of the closure in a sync-closed SCC.
 */
const DSSKripke::Successors &DSSKripke::getSuccessors(const DSSStateKey &state) const {
    if (const auto it {m_successors.find(state)}; it != m_successors.end()) return it->second;
    Successors &successors {m_successors[state]};
    const DSSStateKey divergence {nullptr, &m_divergence};
    if (state.marking == &m_divergence) {
        // The divergence state loops forever without firing a transition
        successors.emplace_back(divergence, nullptr);
        return successors;
    }
    const size_t module {getIdModule()};
    PetriNet *petri {ModularPetriNet::getInstance().getModule(static_cast<int>(module))};

    // 1. Closure (iterative depth-first search, which also detects cycles)
    enum class Color { grey, black };            // grey: on the current path, black: done
    std::unordered_map<DSSStateKey, Color, DSSStateKey::Hash> color;
    std::vector<DSSStateKey> closure;
    bool closure_cycle {false};
    std::vector<std::pair<DSSStateKey, std::vector<DSSStateKey>>> stack;
    const auto invisibleSuccessors = [module](const DSSStateKey &s) {
        std::vector<DSSStateKey> result;
        for (const auto &arc: s.metastate->getSyncSucc()) {
            if (arc->isSynchronised(module)) continue;
            MetaState *dest_ms {arc->getDestination()};
            if (Marking *dest {dest_ms->existMarking(s.marking)}; dest) result.push_back({dest_ms, dest});
        }
        return result;
    };
    color[state] = Color::grey;
    closure.push_back(state);
    stack.emplace_back(state, invisibleSuccessors(state));
    while (!stack.empty()) {
        auto &[current, todo] {stack.back()};
        if (todo.empty()) {
            color[current] = Color::black;
            stack.pop_back();
            continue;
        }
        const DSSStateKey next {todo.back()};
        todo.pop_back();
        const auto it {color.find(next)};
        if (it == color.end()) {
            color[next] = Color::grey;
            closure.push_back(next);
            stack.emplace_back(next, invisibleSuccessors(next));
        } else if (it->second == Color::grey) {
            closure_cycle = true;
        }
    }

    // 2. and 3.
    // Under fairness, only a sync-closed SCC lets the module stop moving forever: the other modules looping
    // (othersCanLoop) or synchronising forever without the module (cycle in the closure) is not enough
    bool diverge {!m_fairness && closure_cycle};
    for (const auto &[ms, m]: closure) {
        SCC *scc {m->getSCCContainer()};
        diverge = diverge || ms->isSyncClosed(scc) || (!m_fairness && ms->othersCanLoop(scc));
        // Local transitions (in the same metastate)
        for (const auto &[t, dest]: m->getListSucc()) {
            successors.push_back({{ms, dest}, t});
        }
        // Synchronisations of the module: sync arcs of the metastate whose source SCC (for this module)
        // is the SCC of m, and whose transition can be fired from m itself
        for (const auto &arc: ms->getSyncSucc()) {
            if (arc->getSourceSCC(module) != scc) continue;
            Transition *t {arc->getFusion()->getTransitionOfModule(static_cast<int>(module))};
            if (t == nullptr) continue;
            petri->setMarking(m);
            if (!t->isLocallyFirable()) continue;
            // The reached marking, in the destination metastate
            t->fire();
            const Marking reached {petri->getMarking()};
            MetaState *dest_ms {arc->getDestination()};
            if (Marking *dest {dest_ms->existMarking(&reached)}; dest) {
                successors.push_back({{dest_ms, dest}, t});
            }
        }
    }
    // The module may stop moving forever: divergence (not a reflexive arc, which would let a finite wait
    // add an empty letter to the word seen by the formula)
    if (diverge) successors.emplace_back(divergence, nullptr);
    return successors;
}
