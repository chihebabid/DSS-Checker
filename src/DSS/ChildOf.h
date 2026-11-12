//
// Created by chiheb on 12/11/2026.
//

#ifndef DISTRIBUTEDSTATESPACE_CHILDOF_H
#define DISTRIBUTEDSTATESPACE_CHILDOF_H

/*
 * @brief Mixin giving a class a (non-owning) link to its parent in the DSS hierarchy:
 *        DistributedSS -> ModuleSS -> MetaState -> ArcSync
 * @tparam Parent type of the parent; a forward declaration is enough
 *
 * The parent may be null: the element is not (or no longer) attached.
 */
template<class Parent>
class ChildOf {
public:
    [[nodiscard]] Parent *getParent() const { return m_parent; }

    void setParent(Parent *parent) { m_parent = parent; }

protected:
    // Only meant to be used as a base class, never deleted through a ChildOf pointer
    ChildOf() = default;
    ~ChildOf() = default;

private:
    Parent *m_parent {nullptr};
};

#endif //DISTRIBUTEDSTATESPACE_CHILDOF_H
