// MarquageGlobal.h: interface for the MarquageGlobal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MARQUAGEGLOBAL_H__32E095E3_E8C6_4893_B5D6_381F7392D41B__INCLUDED_)
#define AFX_MARQUAGEGLOBAL_H__32E095E3_E8C6_4893_B5D6_381F7392D41B__INCLUDED_

#include "DSS/Marking.h"    // Added by ClassView

using std::vector;
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include"ListMarquage.h"
#include "NodeSGSimplified.h"
#include "ArcSG.h"

using std::vector;

class ArcSG;

class TransitionFusionSet;

class NodeSG : public NodeSGSimplified {

public:
    vector<ListMarquage *> m_equiv;

    inline ArcSG *getArc(const int index) { return (*m_arcs)[index]; }

    vector<ArcSG *> *m_arcs;

    long getCountArcs();

    NodeSG(const NodeSG &node);

    NodeSG &operator=(const NodeSG &val);

    bool operator==(NodeSG &marq);


    NodeSG();

    virtual ~NodeSG();


};




inline bool NodeSG::operator==(NodeSG &marq) {

    if (m_list.size() != marq.m_list.size()) return false;
    bool res = true;
    for (unsigned int i = 0; i < m_list.size() && res; i++) {
        res = false;
        if ((m_list.at(i) == marq.m_list.at(i)))
            res = true;
        else if (m_equiv.at(i) != NULL) {
            if (m_equiv.at(i)->existMarquage(&marq.m_list.at(i)))
                res = true;
        } else if (marq.m_equiv.at(i) != NULL) {
            if (marq.m_equiv.at(i)->existMarquage(&(m_list.at(i)))) res = true;
        }
    }
    return res;
}

#endif // !defined(AFX_MARQUAGEGLOBAL_H__32E095E3_E8C6_4893_B5D6_381F7392D41B__INCLUDED_)
