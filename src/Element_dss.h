#ifndef ELEMENT_DSS_H
#define ELEMENT_DSS_H
#include "ProductSCC.h"
#define ListLocalStates vector<Marking*>*
#define ListGlobalStates vector<ListLocalStates>*
#include "Fusion.h"
#include "PElement_dss.h"
class Element_dss : public PElement_dss {
public:
    ProductSCC  *m_product; //ProductSCC
    Element_dss ();
    ~Element_dss();
    Element_dss(const Element_dss &elt) : PElement_dss(elt){
        
        this->m_product=elt.m_product;
    }
    Element_dss & operator=(const Element_dss &elt) {
        PElement_dss::operator =(elt);
        this->m_product=elt.m_product;
        return *this;
    }
    


};

#endif // ELEMENT_DSS_H
