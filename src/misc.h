//
// Created by chiheb on 06/01/24.
//

#ifndef DISTRIBUTEDSTATESPACE_MISC_H
#define DISTRIBUTEDSTATESPACE_MISC_H
#include <vector>
#include <tl/formula.hh>
#include "ListMarquage.h"
#include "TransitionFusionSet.h"
#include "ProductSCC.h"


struct PElement {
    Marking *marquage;
    vector<Transition *> liste_transitions;
};
#define ListLocalStates vector<Marking*>*
#define ListGlobalStates vector<ListLocalStates>*

struct PElement_dss {
    ListGlobalStates m_gs {nullptr};
    TransitionFusionSet *m_fusion {nullptr};
};

/***********************************************************************/
struct element_t {
    Marking marquage;
    vector<Transition *> liste_transitions;
};
/*************************************************************************/
struct Element_dss : public PElement_dss {

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


struct Formula {
    spot::formula f;
    set<string> propositions;
};
#endif //DISTRIBUTEDSTATESPACE_MISC_H
