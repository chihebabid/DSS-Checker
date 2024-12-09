// ListMarquage.h: interface for the Marquage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTMARQUAGE_H__63B24673_B0CC_4C49_9388_0D2377C1C7F6__INCLUDED_)
#define AFX_LISTMARQUAGE_H__63B24673_B0CC_4C49_9388_0D2377C1C7F6__INCLUDED_

#include "DSS/Marking.h"    // Added by ClassView

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <algorithm>

using std::vector;

class ListMarquage {
public:
    ListMarquage()=default;
    virtual ~ListMarquage()=default;
    Marking pop();

    void addGroupe(ListMarquage &groupe);

    ListMarquage(const ListMarquage &elt);

    Marking *getLastMarquage() {
        return &m_liste.back();
    }

    int getCount() { return m_liste.size(); }

    Marking *getMarquage(const int index) { return &m_liste[index]; }

    void addMarquage(Marking *marquage) { if (!existMarquage(marquage)) m_liste.push_back(*marquage); }

    bool existMarquage(Marking *mar) { return (m_liste.end() != find(m_liste.begin(), m_liste.end(), *mar)); }

    bool operator==(ListMarquage &marquage);

    ListMarquage &operator=(const ListMarquage &elt);



private:
    vector<Marking> m_liste;

};

#endif // !defined(AFX_MARQUAGE_H__63B24673_B0CC_4C49_9388_0D2377C1C7F6__INCLUDED_)
