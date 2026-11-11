//
// Created by chiheb on 12/11/2026.
//

#ifndef DISTRIBUTEDSTATESPACE_SCCP_H
#define DISTRIBUTEDSTATESPACE_SCCP_H
#include "SCC.h"

class SCCp {
public:
    SCCp(SCC* scc) : m_data(scc) {}
    SCCp(const SCCp&)=delete;
    SCCp& operator=(const SCCp&)=delete;
private:
    SCC* m_data;
    bool m_sync{};

};


#endif //DISTRIBUTEDSTATESPACE_SCCP_H
