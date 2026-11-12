//
// Created by chiheb on 04/10/23.
//

#include "SpotMC/DSSKripke.h"
#include "SpotMC/DSSState.h"
#include "Petri/Transition.h"
#include "Petri/ModularPetriNet.h"

DSSKripke::DSSKripke(const spot::bdd_dict_ptr &dict_ptr, ModuleSS *metagraph, const std::set<std::string> &aps)
        : spot::twa(dict_ptr), m_metagraph(metagraph) {
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
    return new DSSState(m_metagraph->getMetaState(0)->getListMarkings().at(0));
}

DSSIterator *DSSKripke::succ_iter(const spot::state *s) const {
    return new DSSIterator(static_cast<const DSSState *>(s)->getMarking(), this);
}

std::string DSSKripke::format_state(const spot::state *s) const {
    const auto marking {static_cast<const DSSState *>(s)->getMarking()};
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
 *  3. An edge to the divergence state if the module may stop moving forever: a marking of the closure is in
 *     a sync-closed SCC or in an SCC where another module can loop, or the closure contains a cycle
 *     (the other modules can synchronise forever without the module).
 */
const DSSKripke::Successors &DSSKripke::getSuccessors(Marking *marking) const {
    if (const auto it {m_successors.find(marking)}; it != m_successors.end()) return it->second;
    Successors &successors {m_successors[marking]};
    if (marking == &m_divergence) {
        // The divergence state loops forever without firing a transition
        successors.emplace_back(&m_divergence, nullptr);
        return successors;
    }
    const size_t module {getIdModule()};
    PetriNet *petri {ModularPetriNet::getInstance().getModule(static_cast<int>(module))};

    // 1. Closure (iterative depth-first search, which also detects cycles)
    enum class Color { grey, black };            // grey: on the current path, black: done
    std::unordered_map<Marking *, Color> color;
    std::vector<Marking *> closure;
    bool closure_cycle {false};
    std::vector<std::pair<Marking *, std::vector<Marking *>>> stack;
    const auto invisibleSuccessors = [module](Marking *m) {
        std::vector<Marking *> result;
        for (const auto &arc: m->getSCCContainer()->getParent()->getSyncSucc()) {
            if (arc->isSynchronised(module)) continue;
            if (Marking *dest {arc->getDestination()->existMarking(m)}; dest) result.push_back(dest);
        }
        return result;
    };
    color[marking] = Color::grey;
    closure.push_back(marking);
    stack.emplace_back(marking, invisibleSuccessors(marking));
    while (!stack.empty()) {
        auto &[current, todo] {stack.back()};
        if (todo.empty()) {
            color[current] = Color::black;
            stack.pop_back();
            continue;
        }
        Marking *next {todo.back()};
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
    bool diverge {closure_cycle};
    for (Marking *m: closure) {
        SCC *scc {m->getSCCContainer()};
        diverge = diverge || scc->isSyncClosed() || scc->othersCanLoop();
        // Local transitions
        for (const auto &[t, dest]: m->getListSucc()) {
            successors.emplace_back(dest, t);
        }
        // Synchronisations of the module: sync arcs of the metastate whose source SCC (for this module)
        // is the SCC of m, and whose transition can be fired from m itself
        for (const auto &arc: scc->getParent()->getSyncSucc()) {
            if (arc->getSourceSCC(module) != scc) continue;
            Transition *t {arc->getFusion()->getTransitionOfModule(static_cast<int>(module))};
            if (t == nullptr) continue;
            petri->setMarking(m);
            if (!t->isLocallyFirable()) continue;
            // The reached marking, in the destination metastate
            t->fire();
            const Marking reached {petri->getMarking()};
            if (Marking *dest {arc->getDestination()->existMarking(&reached)}; dest) {
                successors.emplace_back(dest, t);
            }
        }
    }
    // The module may stop moving forever: divergence (not a reflexive arc, which would let a finite wait
    // add an empty letter to the word seen by the formula)
    if (diverge) successors.emplace_back(&m_divergence, nullptr);
    return successors;
}
