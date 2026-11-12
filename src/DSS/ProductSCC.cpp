#include <algorithm>
#include "DSS/ProductSCC.h"
#include "DSS/MetaState.h"

void ProductSCC::addSCC(SCC *scc, const bool any) {
    m_product.push_back({scc, any});
}

SCC *ProductSCC::getSCC(const size_t pos) const {
    return m_product[pos].scc;
}

bool ProductSCC::isAny(const size_t pos) const {
    return m_product[pos].any;
}

// True if an "any" component built on the metastate ms represents scc.
// SCCs are compared by content: equal SCCs may belong to distinct metastate objects.
static bool represents(MetaState *ms, const SCC &scc) {
    const auto list_sccs {ms->getListSCCs()};
    return std::any_of(list_sccs->begin(), list_sccs->end(), [&scc](SCC *elt) { return elt->isEqual(scc); });
}

std::partial_ordering ProductSCC::compare(const Component &a, const Component &b) {
    if (!a.any && !b.any) {
        return a.scc->isEqual(*b.scc) ? std::partial_ordering::equivalent : std::partial_ordering::unordered;
    }
    MetaState *ms_a {a.scc->getParent()}, *ms_b {b.scc->getParent()};
    if (a.any && b.any) {
        return (ms_a == ms_b || *ms_a == *ms_b) ? std::partial_ordering::equivalent : std::partial_ordering::unordered;
    }
    if (a.any) {
        return represents(ms_a, *b.scc) ? std::partial_ordering::greater : std::partial_ordering::unordered;
    }
    return represents(ms_b, *a.scc) ? std::partial_ordering::less : std::partial_ordering::unordered;
}

std::partial_ordering operator<=>(const ProductSCC &a, const ProductSCC &b) {
    if (a.m_product.size() != b.m_product.size()) return std::partial_ordering::unordered;
    auto result {std::partial_ordering::equivalent};
    for (size_t module {}; module < a.m_product.size(); ++module) {
        const auto cmp {ProductSCC::compare(a.m_product[module], b.m_product[module])};
        if (cmp == std::partial_ordering::unordered) return cmp;
        if (cmp == std::partial_ordering::equivalent) continue;
        // a is greater on one module and less on another one (a-c and -bc)
        if (result != std::partial_ordering::equivalent && result != cmp) return std::partial_ordering::unordered;
        result = cmp;
    }
    return result;
}

size_t ProductSCC::getRepresentedProductsCount(const size_t module) const {
    size_t count {1};
    for (size_t index{}; const auto &[scc, any] : m_product) {
        if (index!=module) {
            if (any) {
                count*=scc->getParent()->getTerminalSCCsCount();
            }
            else if (!scc->isTerminal()) {
                return 0;
            }
        }
        ++index;
    }
    return count;
}


