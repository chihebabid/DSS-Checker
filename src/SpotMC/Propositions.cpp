//
// Atomic propositions of a formula: transition names (action-based) or conditions on the
// marking of a place (state-based).
//

#include <regex>
#include "SpotMC/Propositions.h"
#include "DSS/Marking.h"
#include "Petri/ModularPetriNet.h"
#include "Petri/PetriNet.h"

namespace {
    const std::string identifier {R"([A-Za-z_][A-Za-z0-9_]*)"};
    const std::string comparison_op {R"(==|!=|<=|>=|<|>)"};

    // "place op number", the whole atomic proposition
    const std::regex comparison_ap {R"(^\s*()" + identifier + R"()\s*()" + comparison_op + R"()\s*([0-9]+)\s*$)"};

    // "place op number" inside a formula
    const std::regex comparison_in_formula {"(" + identifier + R"()\s*()" + comparison_op + R"()\s*([0-9]+))"};

    std::optional<StatePredicate::Op> parseOp(const std::string &op) {
        if (op == "==") return StatePredicate::Op::eq;
        if (op == "!=") return StatePredicate::Op::ne;
        if (op == "<") return StatePredicate::Op::lt;
        if (op == "<=") return StatePredicate::Op::le;
        if (op == ">") return StatePredicate::Op::gt;
        if (op == ">=") return StatePredicate::Op::ge;
        return std::nullopt;
    }
}

bool StatePredicate::holds(const Marking &marking) const {
    const uint32_t tokens {marking.getTokens(place)};
    switch (op) {
        case Op::eq: return tokens == value;
        case Op::ne: return tokens != value;
        case Op::lt: return tokens < value;
        case Op::le: return tokens <= value;
        case Op::gt: return tokens > value;
        case Op::ge: return tokens >= value;
    }
    return false;
}

std::string quoteComparisons(const std::string &formula) {
    std::string result;
    size_t pos {};
    while (pos < formula.size()) {
        const size_t quote {formula.find('"', pos)};
        // Text before the next quoted string: quote its comparisons
        const std::string unquoted {formula.substr(pos, quote == std::string::npos ? std::string::npos : quote - pos)};
        result += std::regex_replace(unquoted, comparison_in_formula, "\"$1 $2 $3\"");
        if (quote == std::string::npos) break;
        // Quoted string: copied as it is
        const size_t end {formula.find('"', quote + 1)};
        if (end == std::string::npos) {
            result += formula.substr(quote);
            break;
        }
        result += formula.substr(quote, end - quote + 1);
        pos = end + 1;
    }
    return result;
}

std::optional<ResolvedPropositions> resolvePropositions(const std::set<std::string> &aps) {
    auto &modular {ModularPetriNet::getInstance()};
    for (size_t module {}; module < ModularPetriNet::getModuleCount(); ++module) {
        PetriNet *petri {modular.getModule(static_cast<int>(module))};
        ResolvedPropositions resolved {module, {}, {}};
        bool all_found {true};
        for (const auto &ap: aps) {
            std::smatch match;
            if (std::regex_match(ap, match, comparison_ap)) {
                const int place {petri->getPlaceIndex(match[1])};
                if (place < 0) {
                    all_found = false;
                    break;
                }
                resolved.predicates.push_back({ap, static_cast<size_t>(place), *parseOp(match[2]),
                                               static_cast<uint32_t>(std::stoul(match[3]))});
            } else if (petri->getTransitionAddress(ap) != nullptr) {
                resolved.transitions.insert(ap);
            } else if (const int place {petri->getPlaceIndex(ap)}; place >= 0) {
                resolved.predicates.push_back({ap, static_cast<size_t>(place), StatePredicate::Op::ge, 1});
            } else {
                all_found = false;
                break;
            }
        }
        if (all_found) return resolved;
    }
    return std::nullopt;
}
