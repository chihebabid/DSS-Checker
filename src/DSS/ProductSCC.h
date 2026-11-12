#ifndef PRODUCTSCC_H
#define PRODUCTSCC_H

#include <compare>
#include <vector>
#include "DSS/SCC.h"

/*
 * A product of SCCs: one component per module.
 * A component is either a given SCC, or "any" (displayed as "-"): it then represents
 * every SCC of a metastate of the module.
 */
class ProductSCC {
public:
    ProductSCC()=default;
    virtual ~ProductSCC()=default;
    // Add the component of the next module.
    // If any is true, the component represents every SCC of the metastate of scc ("-")
    void addSCC(SCC *scc, bool any = false);
    // For an "any" component, returns the SCC given to addSCC (it identifies the metastate)
    [[nodiscard]] SCC *getSCC(size_t pos) const;
    // True if the component of module pos is "any" ("-")
    [[nodiscard]] bool isAny(size_t pos) const;
    // Number of products of terminal SCCs of the modules other than module represented by this product:
    // an "any" component stands for every terminal SCC of its metastate, another one counts for one
    // if it is terminal, else the product represents nothing
    size_t getRepresentedProductsCount(size_t module) const;

    /*
     * Partial order: a > b if a represents strictly more products than b
     *  - equivalent : all components are the same
     *  - greater    : a and b only differ on components where a is "-" and represents the SCC of b (a-c > abc)
     *  - less       : the converse (abc < ab-)
     *  - unordered  : otherwise (abc and abd; ab- and abc when "-" does not represent c; a-c and -bc)
     */
    friend std::partial_ordering operator<=>(const ProductSCC &a, const ProductSCC &b);

    friend bool operator==(const ProductSCC &a, const ProductSCC &b) {
        return (a <=> b) == std::partial_ordering::equivalent;
    }
private:
    struct Component {
        SCC *scc;
        bool any;
    };
    static std::partial_ordering compare(const Component &a, const Component &b);
    std::vector<Component> m_product;
};

#endif // PRODUCTSCC_H
