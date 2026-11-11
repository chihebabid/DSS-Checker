#include "DSS/ProductSCC.h"

void ProductSCC::addSCC(SCC *scc) {
    m_product.emplace_back(scc);
}


bool ProductSCC::operator==(const ProductSCC &product) const {
    bool result {true};
    for (size_t module {}; module < m_product.size() && result; ++module) {
        result = m_product[module]->isEqual(*product.m_product[module]);
    }
    return result;
}

SCC *ProductSCC::getSCC(const size_t pos) {
    return m_product[pos];
}

size_t ProductSCC::getRepresentedProductsCount(const TransitionFusionSet &fusion,size_t module) const {
    size_t count {1};
    for (size_t index{}; const auto &scc : m_product) {
        if (index!=module) {
            if (fusion.getTransitionOfModule(index)==nullptr) {
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


