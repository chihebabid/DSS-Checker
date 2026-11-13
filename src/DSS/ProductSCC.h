#ifndef PRODUCTSCC_H
#define PRODUCTSCC_H

#include <compare>
#include <cstdint>
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
    ~ProductSCC()=default;
    // Add the component of the next module.
    // If any is true, the component represents every SCC of the metastate of scc ("-")
    void addSCC(SCC *scc, bool any = false);
    // For an "any" component, returns the SCC given to addSCC (it identifies the metastate)
    [[nodiscard]] SCC *getSCC(size_t pos) const;
    // Replace the SCC of component pos by scc, which must contain the same markings (another copy of it):
    // the product is unchanged (same "any" flag, same hash)
    void replaceSCC(size_t pos, SCC *scc);
    // True if the component of module pos is "any" ("-")
    [[nodiscard]] bool isAny(size_t pos) const;
    // Hash consistent with operator== ("any" components are all equivalent); cached
    [[nodiscard]] size_t hash() const {
        return m_hash != 0 ? m_hash : computeHash();
    }

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
    /*
     * A component, on 8 bytes (there is one per module in each sync arc): the SCC pointer, whose lowest bit
     * (always 0, SCCs being aligned) is set for an "any" component
     */
    class Component {
    public:
        Component(SCC *scc, bool any)
            : m_bits {reinterpret_cast<std::uintptr_t>(scc) | static_cast<std::uintptr_t>(any)} {}
        [[nodiscard]] SCC *scc() const { return reinterpret_cast<SCC *>(m_bits & ~std::uintptr_t {1}); }
        [[nodiscard]] bool any() const { return m_bits & 1; }
    private:
        std::uintptr_t m_bits;
    };
    static std::partial_ordering compare(const Component &a, const Component &b);
    size_t computeHash() const;
    std::vector<Component> m_product;
    mutable size_t m_hash {};          // Cache of hash(); 0: not computed yet
};

#endif // PRODUCTSCC_H
