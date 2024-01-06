#include "ProductSCC.h"

void ProductSCC::addSCC(SCC *scc) {
    m_product.push_back(scc);
}


bool ProductSCC::operator==(const ProductSCC &product) {
    bool result = true;
    for (int module = 0; module < m_product.size() && result; module++) {
        result = m_product.at(module)->isEqual(*product.m_product.at(module));
    }
    return result;
}

SCC *ProductSCC::getSCC(const int pos) {
    return m_product.at(pos);
}

ProductSCC &ProductSCC::operator=(const ProductSCC &product) {
    if (&product == this) return *this;
    this->m_product = product.m_product;
    return *this;
}
