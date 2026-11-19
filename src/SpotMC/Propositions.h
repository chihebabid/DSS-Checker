//
// Atomic propositions of a formula: transition names (action-based) or conditions on the
// marking of a place (state-based).
//

#ifndef DISTRIBUTEDSTATESPACE_PROPOSITIONS_H
#define DISTRIBUTEDSTATESPACE_PROPOSITIONS_H

#include <cstddef>
#include <cstdint>
#include <optional>
#include <set>
#include <string>
#include <vector>

class Marking;

// A state-based atomic proposition: "place op value", e.g. "client_idle_1 >= 2".
// A bare place name p stands for "p >= 1" (p is marked).
struct StatePredicate {
    enum class Op { eq, ne, lt, le, gt, ge };

    std::string name;   // Name of the atomic proposition in the formula
    size_t place {};    // Index of the place in the markings of the module
    Op op {Op::ge};
    uint32_t value {1};

    [[nodiscard]] bool holds(const Marking &marking) const;
};

// Atomic propositions of a formula, resolved on the module against which it is checked
struct ResolvedPropositions {
    size_t module {};
    std::set<std::string> transitions;        // Action-based propositions
    std::vector<StatePredicate> predicates;   // State-based propositions
};

// Quotes the comparisons "place op number" written outside double quotes (op: == != <= >= < >), so that
// Spot reads each one as an atomic proposition: a >= 2 && b becomes "a >= 2" && b. This accepts the
// LTSmin syntax of state-based formulas.
std::string quoteComparisons(const std::string &formula);

// Resolves the atomic propositions aps on the first module having all of them:
//  - "place op number": a condition on a place of the module;
//  - a name: a transition of the module if there is one, otherwise a place of the module (p means p >= 1).
// Returns nullopt if no module has all of them.
std::optional<ResolvedPropositions> resolvePropositions(const std::set<std::string> &aps);

#endif //DISTRIBUTEDSTATESPACE_PROPOSITIONS_H
