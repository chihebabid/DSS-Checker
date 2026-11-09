// NodeSGSimplified.h: interface for the NodeSGSimplified class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NODESGSIMPLIFIED_H__C0E45B46_BE36_4B2F_A970_B674E7C897CF__INCLUDED_)
#define AFX_NODESGSIMPLIFIED_H__C0E45B46_BE36_4B2F_A970_B674E7C897CF__INCLUDED_

#include "DSS/Marking.h"    // Added by ClassView

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class NodeSGSimplified {
public:
    NodeSGSimplified(const NodeSGSimplified &)=default;

    NodeSGSimplified& operator=(const NodeSGSimplified &)=default;

    void addMarking(Marking &marking);

    Marking *getMarking(const int index) { return &m_list[index]; }

    void setEmpty(const int index);

    bool operator==(NodeSGSimplified &other);

    NodeSGSimplified()=default;

    virtual ~NodeSGSimplified()=default;

    vector<Marking> m_list;
protected:

};


inline void NodeSGSimplified::setEmpty(const int index) {
    //m_list[index].setEmpty(true);
}


#endif // !defined(AFX_NODESGSIMPLIFIED_H__C0E45B46_BE36_4B2F_A970_B674E7C897CF__INCLUDED_)
