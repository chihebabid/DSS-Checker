#include <algorithm>
#include "DSS/ProductSCC.h"
#include "DSS/MetaState.h"

void ProductSCC::addSCC(SCC *scc, const bool any) {
    m_product.push_back({scc, any});
    m_hash_valid = false;
}

size_t ProductSCC::computeHash() const {
    size_t hash {m_product.size()};
    for (const auto &[scc, any]: m_product) {
        const size_t h {any ? 0x9e3779b97f4a7c15ULL : scc->contentHash()};
        hash = hash * 1099511628211ULL ^ h;
    }
    m_hash = hash;
    m_hash_valid = true;
    return m_hash;
}

SCC *ProductSCC::getSCC(const size_t pos) const {
    return m_product[pos].scc;
}

bool ProductSCC::isAny(const size_t pos) const {
    return m_product[pos].any;
}

std::partial_ordering ProductSCC::compare(const Component &a, const Component &b) {
    if (!a.any && !b.any) {
        return a.scc->isEqual(*b.scc) ? std::partial_ordering::equivalent : std::partial_ordering::unordered;
    }
    if (a.any && b.any) {
        return std::partial_ordering::equivalent;
    }
    if (a.any) {
        return std::partial_ordering::greater;
    }
    return std::partial_ordering::less;
}

std::partial_ordering operator<=>(const ProductSCC &a, const ProductSCC &b) {
    if (a.m_product.size() != b.m_product.size()) return std::partial_ordering::unordered;
    auto result {std::partial_ordering::equivalent};
    for (size_t module {}; module < a.m_product.size(); ++module) {
        const auto cmp {ProductSCC::compare(a.m_product[module], b.m_product[module])};
        if (cmp == std::partial_ordering::unordered) return std::partial_ordering::unordered;
        if (cmp == std::partial_ordering::equivalent) continue;
        // a is greater on one module and less on another one (a-c and -bc)
        if (result != std::partial_ordering::equivalent && result != cmp) {
            return std::partial_ordering::unordered;
        }
        result = cmp;
    }
    return result;
}

