//
// Created by chiheb on 06/01/24.
//

#ifndef DISTRIBUTEDSTATESPACE_MISC_H
#define DISTRIBUTEDSTATESPACE_MISC_H
#include <vector>
#include "Fils.h"
#include "ListMarquage.h"
#include "Fusion.h"

class ElementRed {
public:

    void addFils(vector<Fils> &liste)  {
        for (int i = 0; liste_fils.size(); i++) {
            if (!isExist(&liste.at(i).getMarquage()))
                liste_fils.push_back(liste.at(i));
        }
    }

    ElementRed(const ElementRed &elt) {
        {
            this->groupe = elt.groupe;
            this->etat = elt.etat;
            this->liste_fils = elt.liste_fils;
        }
    }

    ElementRed &operator=(const ElementRed &elt) {
        if (this == &elt) return *this;
        this->groupe = elt.groupe;
        this->etat = elt.etat;
        this->liste_fils = elt.liste_fils;
        return *this;
    }

    bool isExist(Marking *marq) {
        return groupe->existMarquage(marq);
    }

    ListMarquage *groupe;
    std::vector<Fils> liste_fils;
    bool etat;
    ElementRed()=default;
    virtual ~ElementRed()=default;
};

struct PElement {
    Marking *marquage;
    vector<Transition *> liste_transitions;
};
#define ListLocalStates vector<Marking*>*
#define ListGlobalStates vector<ListLocalStates>*

class PElement_dss {
public:
    PElement_dss() {
        m_gs = nullptr;
        m_fusion = nullptr;
    }

    PElement_dss(const PElement_dss &elt) {
        this->m_fusion = elt.m_fusion;
        this->m_gs = elt.m_gs;
    }

    virtual PElement_dss &operator=(const PElement_dss &elt) {
        this->m_gs = elt.m_gs;
        this->m_fusion = elt.m_fusion;
        return *this;
    }

    virtual ~PElement_dss()=default;

    ListGlobalStates m_gs;
    Fusion *m_fusion;
};
/********************************************************************************/
class ElementPhase1 {
public:
    ElementPhase1(const ElementPhase1 &elt) {
        this->groupe = elt.groupe;
        this->etat = elt.etat;
        this->liste_fils = elt.liste_fils;
    }

    bool etat;

    bool isExist(Marking *marq)  {
        return groupe->existMarquage(marq);
    }

    ElementPhase1()=default;

    virtual ~ElementPhase1()=default;

    ListMarquage *groupe;
    ListMarquage liste_fils;

    ElementPhase1 &operator=(const ElementPhase1 &elt)  {
        this->groupe = elt.groupe;
        this->liste_fils = elt.liste_fils;
        this->etat = elt.etat;
        return *this;
    }
};

struct PilePhase1 {
public:
    long isCycle(Marking *marq) {
        for (long i = 0; i < m_liste.size(); i++) {
            if (m_liste.at(i).groupe->existMarquage(marq)) return i;
        }
        return -1L;
    }

    std::vector<ElementPhase1> m_liste;

};
/***********************************************************************/
struct element_t {
    Marking marquage;
    vector<Transition *> liste_transitions;
};
#endif //DISTRIBUTEDSTATESPACE_MISC_H
