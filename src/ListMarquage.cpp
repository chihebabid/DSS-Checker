// ListMarquage.cpp: implementation of the Marquage class.
//
//////////////////////////////////////////////////////////////////////


#include "ListMarquage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


bool ListMarquage::operator==(ListMarquage &listemarquage) {

    if (listemarquage.m_liste.size() == m_liste.size()) {

        for (int i = 0; i < m_liste.size(); i++) {
            if (!(existMarquage(&listemarquage.m_liste[i]))) return false;
        }
        return true;
    }
    return false;
}


void ListMarquage::addGroupe(ListMarquage &groupe) {
    for (int i = 0; i < groupe.getCount(); i++)
        addMarquage(groupe.getMarquage(i));
}

Marking ListMarquage::pop() {
    Marking marq = m_liste.back();
    m_liste.pop_back();
    return marq;
}
