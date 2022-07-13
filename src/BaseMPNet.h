//
// Created by chiheb on 13/07/22.
//

#ifndef DISTRIBUTEDSTATESPACE_BASEMPNET_H
#define DISTRIBUTEDSTATESPACE_BASEMPNET_H
#include "PetriNet.h"

class BaseMPNet {
public:
    PetriNet* getModule(const int index);
    ~BaseMPNet();
protected:
    vector<PetriNet*> m_modules;
};


#endif //DISTRIBUTEDSTATESPACE_BASEMPNET_H
