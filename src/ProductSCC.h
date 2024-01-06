#ifndef PRODUCTSCC_H
#define PRODUCTSCC_H

#include "DSS/SCC.h"

class ProductSCC {
public:
    ProductSCC()=default;

    void addSCC(SCC *scc);

    virtual ~ProductSCC()=default;

    bool operator==(const ProductSCC &product);

    ProductSCC &operator=(const ProductSCC &product);

    SCC *getSCC(const int pos);

protected:

private:
    vector<SCC *> m_product;
};

#endif // PRODUCTSCC_H
