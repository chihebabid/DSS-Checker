//
// Created by chiheb on 06/01/24.
//

#ifndef DISTRIBUTEDSTATESPACE_MISC_H
#define DISTRIBUTEDSTATESPACE_MISC_H
#include <vector>
#include "ListMarquage.h"
#include "Fusion.h"
#include "ProductSCC.h"


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

/***********************************************************************/
struct element_t {
    Marking marquage;
    vector<Transition *> liste_transitions;
};
/*************************************************************************/
class Element_dss : public PElement_dss {
public:
    ProductSCC *m_product {nullptr};
};
/**********************************************************************************/
class RElement_dss : public PElement_dss {
public:
    //ProductSCC
    RElement_dss():PElement_dss() {}

    ~RElement_dss()=default;

    RElement_dss(const RElement_dss &elt) : PElement_dss(elt) {

        this->ml_metastates = elt.ml_metastates;
    }

    RElement_dss &operator=(const RElement_dss &elt) {
        PElement_dss::operator=(elt);
        this->ml_metastates = elt.ml_metastates;
        return *this;
    }

    MetaState *getMetaState(int i) const {
        return ml_metastates[i];
    }

    void addMetaState(MetaState *ms) {
        ml_metastates.push_back(ms);
    }

    void setListMetaStates(vector<MetaState *> &l_metastates) {
        ml_metastates = l_metastates;
    }

private:
    vector<MetaState *> ml_metastates;
};



#endif //DISTRIBUTEDSTATESPACE_MISC_H
