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

#endif //DISTRIBUTEDSTATESPACE_MISC_H
