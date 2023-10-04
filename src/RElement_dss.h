#ifndef RELEMENT_DSS_H
#define RELEMENT_DSS_H

#define ListLocalStates vector<Marking*>*
#define ListGlobalStates vector<ListLocalStates>*

#include "ProductSCC.h"
#include "PElement_dss.h"

class RElement_dss : public PElement_dss {
public:
    //ProductSCC
    RElement_dss();

    ~RElement_dss();

    RElement_dss(const RElement_dss &elt) : PElement_dss(elt) {

        this->ml_metastates = elt.ml_metastates;
    }

    RElement_dss &operator=(const RElement_dss &elt) {
        PElement_dss::operator=(elt);
        this->ml_metastates = elt.ml_metastates;
        return *this;
    }

    MetaState *getMetaState(int i) const;

    void addMetaState(MetaState *ms);

    void setListMetaStates(vector<MetaState *> &l_metastates);

private:
    vector<MetaState *> ml_metastates;


};

#endif // RELEMENT_DSS_H
