#include <algorithm>
//
// Created by chiheb on 04/10/23.
//

#include "SpotMC/DSSIterator.h"
#include "Petri/Transition.h"
#include "SpotMC/DSSKripke.h"
#include "Petri/ModularPetriNet.h"
DSSIterator::DSSIterator(Marking *marking, const DSSKripke *kripke) : m_kripke(kripke) {
    Marking *divergence {kripke->getDivergence()};
    if (marking == divergence) {
        // The divergence state loops forever without firing a transition
        m_succ.emplace_back(divergence, nullptr);
        return;
    }

    SCC *scc {marking->getSCCContainer()};
    const size_t module {kripke->getIdModule()};
    // The module may stay forever in scc: if scc is sync-closed, or if a sync arc leaving scc is marked
    // divergent (another module may loop forever, so the synchronisation may never happen)
    const auto &sync_arcs {scc->getParent()->getSyncSucc()};
    const bool divergent_arc {std::any_of(sync_arcs.begin(), sync_arcs.end(), [scc, module](const auto &arc) {
        return arc->isDivergent() && arc->getSourceSCC(module) == scc;
    })};
    if (scc->isSyncClosed() || divergent_arc) {
        m_succ.emplace_back(divergence, nullptr);
    }

    // Local transitions
    for (const auto &[t, m]: marking->getListSucc()) {
        m_succ.emplace_back(m, t);
    }

    // Synchronisations: sync arcs of the metastate whose source SCC (for this module) is the SCC of marking,
    // and whose transition can be fired from marking itself
    PetriNet *petri {ModularPetriNet::getInstance().getModule(static_cast<int>(module))};
    for (const auto &arc: scc->getParent()->getSyncSucc()) {
        if (arc->getSourceSCC(module) != scc) continue;
        Transition *t {arc->getFusion()->getTransitionOfModule(static_cast<int>(module))};
        petri->setMarking(marking);
        if (t == nullptr || !t->isLocallyFirable()) continue;
        // The reached marking, in the destination metastate
        t->fire();
        const Marking reached {petri->getMarking()};
        if (Marking *dest {arc->getDestination()->existMarking(&reached)}; dest) {
            m_succ.emplace_back(dest, t);
        }
    }
}


bool DSSIterator::first() {
    m_current_edge = 0;
    return m_succ.size() != 0;              // There exists a successor.
}

bool DSSIterator::next() {
    ++m_current_edge;
    return m_current_edge < m_succ.size();          // More successors?
}

bool DSSIterator::done() const {
    return m_current_edge == m_succ.size();
}

DSSState *DSSIterator::dst() const {
    return new DSSState(m_succ[m_current_edge].first);
}

bdd DSSIterator::cond() const {
    return m_kripke->edgeCondition(m_succ[m_current_edge].second);
}
