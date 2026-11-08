// ListMarking.cpp: implementation of the ListMarking class.
//
//////////////////////////////////////////////////////////////////////


#include "ListMarking.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


bool ListMarking::operator==(ListMarking &other) {
    if (other.m_list.size() == m_list.size()) {
        for (int i = 0; i < m_list.size(); i++) {
            if (!(existMarking(&other.m_list[i]))) return false;
        }
        return true;
    }
    return false;
}


void ListMarking::addGroup(ListMarking &group) {
    for (int i = 0; i < group.getCount(); i++)
        addMarking(group.getMarking(i));
}

Marking ListMarking::pop() {
    Marking marking {m_list.back()};
    m_list.pop_back();
    return marking;
}
