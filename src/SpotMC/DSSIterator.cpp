//
// Created by chiheb on 04/10/23.
//

#include "DSSIterator.h"


bool DSSIterator::first()
{
    m_current_edge=0;
    return m_edge_count!=0;              // There exists a successor.
}

bool DSSIterator::next()
{
    m_current_edge++;
    return m_current_edge<m_lsucc.size();          // More successors?
}

bool DSSIterator::done() const {

    return m_current_edge==m_lsucc.size();
}

DSSState* DSSIterator::dst() const
{
    return new DSSState(std::get<0>(m_lsucc[m_current_edge]),std::get<1>(m_lsucc[m_current_edge]));
}