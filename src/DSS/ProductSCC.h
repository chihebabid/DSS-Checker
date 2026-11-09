#ifndef PRODUCTSCC_H
#define PRODUCTSCC_H

#include "DSS/SCC.h"
#include "Petri/TransitionFusionSet.h"
class ProductSCC {
public:
    ProductSCC()=default;
    virtual ~ProductSCC()=default;
    void addSCC(SCC *scc);
    bool operator==(const ProductSCC &product) const;
    SCC *getSCC(const size_t pos);
    size_t getRepresentedProductsCount(const TransitionFusionSet &fusion,size_t module) const;
private:
    vector<SCC *> m_product;
};

#endif // PRODUCTSCC_H
