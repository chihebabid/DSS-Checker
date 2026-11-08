// ListMarking.h: interface for the ListMarking class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTMARKING_H__63B24673_B0CC_4C49_9388_0D2377C1C7F6__INCLUDED_)
#define AFX_LISTMARKING_H__63B24673_B0CC_4C49_9388_0D2377C1C7F6__INCLUDED_

#include "DSS/Marking.h"    // Added by ClassView

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <algorithm>

using std::vector;

class ListMarking {
public:
    ListMarking()=default;
    virtual ~ListMarking()=default;
    Marking pop();

    void addGroup(ListMarking &group);

    ListMarking(const ListMarking &elt)=default;
    ListMarking &operator=(const ListMarking &elt)=default;

    Marking *getLastMarking() {
        return &m_list.back();
    }

    int getCount() { return m_list.size(); }

    Marking *getMarking(const int index) { return &m_list[index]; }

    void addMarking(Marking *marking) { if (!existMarking(marking)) m_list.push_back(*marking); }

    bool existMarking(Marking *mar) { return (m_list.end() != find(m_list.begin(), m_list.end(), *mar)); }

    bool operator==(ListMarking &other);

private:
    vector<Marking> m_list;

};

#endif // !defined(AFX_LISTMARKING_H__63B24673_B0CC_4C49_9388_0D2377C1C7F6__INCLUDED_)
