//
// Created by chiheb on 11/12/24.
//

#include "MarkingArray.h"


MarkingArray::~MarkingArray() {
    if (m_marking) delete []m_marking;
}

MarkingArray::MarkingArray(size_t s):m_size(s)  {
    m_marking=new unsigned char[m_size];
}

byte_t& MarkingArray::operator[](const size_t index) {
    return m_marking[index];
}
size_t MarkingArray::size() const {
    return m_size;
}

bool MarkingArray::operator==(const MarkingArray &other) const {
    return  (m_size!=other.m_size) ? false : std::memcmp(m_marking,other.m_marking,m_size)==0;
}

MarkingArray::MarkingArray(const MarkingArray &other) {
    m_size=other.m_size;
    m_marking=new byte_t[m_size];
    memcpy(m_marking,other.m_marking,m_size);
}

MarkingArray::MarkingArray(MarkingArray &&other) {
    m_marking=other.m_marking;
    m_size=other.m_size;
    other.m_marking=nullptr;
    other.m_size=0;
}

MarkingArray& MarkingArray::operator=(const MarkingArray &other) {
    if (this==&other) return *this;
    memcpy(m_marking,other.m_marking,m_size);
    return *this;
}
MarkingArray& MarkingArray::operator=(MarkingArray &&other) {
    if (this==&other) return *this;
    if (m_marking) delete []m_marking;
    m_marking=other.m_marking;
    m_size=other.m_size;
    other.m_marking=nullptr;
    other.m_size=0;
    return *this;
}