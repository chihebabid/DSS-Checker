//
// Created by chiheb on 06/01/24.
//

#ifndef DISTRIBUTEDSTATESPACE_MISC_H
#define DISTRIBUTEDSTATESPACE_MISC_H
#include <vector>
#include <tl/formula.hh>
#include "DSS/Marking.h"
#include "Petri/TransitionFusionSet.h"
#include "DSS/ProductSCC.h"


#define ListLocalStates vector<Marking*>*
#define ListGlobalStates vector<ListLocalStates>*

struct PElement_dss {
    ListGlobalStates m_gs {nullptr};
    TransitionFusionSet *m_fusion {nullptr};
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

        this->m_metastates = elt.m_metastates;
    }

    RElement_dss &operator=(const RElement_dss &elt) {
        PElement_dss::operator=(elt);
        this->m_metastates = elt.m_metastates;
        return *this;
    }

    MetaState *getMetaState(int i) const {
        return m_metastates[i];
    }

    void addMetaState(MetaState *ms) {
        m_metastates.push_back(ms);
    }

    void setListMetaStates(vector<MetaState *> &l_metastates) {
        m_metastates = l_metastates;
    }

private:
    vector<MetaState *> m_metastates;
};


struct Formula {
    spot::formula f;
    set<string> propositions;
};
#endif //DISTRIBUTEDSTATESPACE_MISC_H
