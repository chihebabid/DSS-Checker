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
