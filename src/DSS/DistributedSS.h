//
// Created by chiheb on 11/11/2026.
//

#ifndef DISTRIBUTEDSTATESPACE_DISTRIBUTEDSS_H
#define DISTRIBUTEDSTATESPACE_DISTRIBUTEDSS_H

#include <vector>
#include <cstddef>
#include "DSS/ModuleSS.h"

/*
 * @brief Distributed state space: owns one ModuleSS per module of the modular Petri net
 */
class DistributedSS {
public:
    using container_t = std::vector<ModuleSS *>;

    DistributedSS() = default;
    ~DistributedSS();

    DistributedSS(const DistributedSS &) = delete;
    DistributedSS &operator=(const DistributedSS &) = delete;

    void addModule(ModuleSS *module);

    ModuleSS *getModule(size_t index) const;

    ModuleSS *operator[](size_t index) const { return m_modules_list[index]; }

    size_t getModuleCount() const { return m_modules_list.size(); }

    container_t::const_iterator begin() const { return m_modules_list.cbegin(); }

    container_t::const_iterator end() const { return m_modules_list.cend(); }

private:
    container_t m_modules_list;
};


#endif //DISTRIBUTEDSTATESPACE_DISTRIBUTEDSS_H
