#include "ProductSCC.h"

void ProductSCC::addSCC(SCC *scc) {
    m_product.push_back(scc);
}


bool ProductSCC::operator==(const ProductSCC &product) {
    bool result {true};
    for (size_t module {}; module < m_product.size() && result; ++module) {
        result = m_product[module]->isEqual(*product.m_product[module]);
    }
    return result;
}

SCC *ProductSCC::getSCC(const size_t pos) {
    return m_product[pos];
}


