#ifndef PRODUCTSCC_H
#define PRODUCTSCC_H
#include "SCC.h"

class ProductSCC {
    public:
        ProductSCC();
        void addSCC(SCC* scc);
        virtual ~ProductSCC();
        bool operator == (const ProductSCC &product);
        ProductSCC & operator=(const ProductSCC &product);
        SCC * getSCC(const int pos);
    protected:

    private:
        vector<SCC*> m_product;
};

#endif // PRODUCTSCC_H
