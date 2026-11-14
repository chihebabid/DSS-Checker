#ifndef DISTRIBUTEDSTATESPACE_MARKING_H
#define DISTRIBUTEDSTATESPACE_MARKING_H

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <utility>
#include <vector>

class Transition;

class SCC;

/*
 * A marking of a (module) Petri net: one token count per place, stored on 8 bits.
 * It is also a node of the local reachability graph: it knows its successors and the SCC containing it.
 */
class Marking {
public:
    using token_t = unsigned char;
    using succ_list_t = std::vector<std::pair<Transition *, Marking *>>;

    // Marking of place_count places, all empty
    explicit Marking(size_t place_count);

    // Copy only copies the token counts: the copy is a new node without successors nor SCC
    Marking(const Marking &other);

    // Only assigns the token counts: successors and SCC of this node are left unchanged
    Marking &operator=(const Marking &other);

    Marking(Marking &&) noexcept = default;

    Marking &operator=(Marking &&) noexcept = default;

    ~Marking() = default;

    [[nodiscard]] size_t getPlaceCount() const { return m_data.size(); }

    [[nodiscard]] token_t getTokens(size_t place) const { return m_data[place]; }

    // @throws std::overflow_error if tokens does not fit on 8 bits
    void setTokens(size_t place, uint32_t tokens);

    bool operator==(const Marking &other) const { return m_data == other.m_data; }

    // Hash of the token counts (consistent with operator==)
    [[nodiscard]] size_t hash() const;

    void addSucc(Transition *t, Marking *m);

    succ_list_t &getListSucc() { return m_succ; }

    [[nodiscard]] const succ_list_t &getListSucc() const { return m_succ; }

    [[nodiscard]] SCC *getSCCContainer() const { return m_scc_container; }

    // Position of the marking in its local graph (insertion order), set by LocalGraph::insertMarking()
    [[nodiscard]] uint32_t getPosition() const { return m_position; }

    void setPosition(uint32_t position) { m_position = position; }

    void setSCCContainer(SCC *c) { m_scc_container = c; }

    // Prints the token counts, e.g. (1,0,2)
    friend std::ostream &operator<<(std::ostream &stream, const Marking &m);

    // Functors to store Marking pointers in hashed containers, compared by token counts
    struct PtrHash {
        size_t operator()(const Marking *m) const { return m->hash(); }
    };

    struct PtrEqual {
        bool operator()(const Marking *a, const Marking *b) const { return *a == *b; }
    };

private:
    std::vector<token_t> m_data;
    succ_list_t m_succ;
    SCC *m_scc_container {nullptr};
    uint32_t m_position {};
};

#endif //DISTRIBUTEDSTATESPACE_MARKING_H
