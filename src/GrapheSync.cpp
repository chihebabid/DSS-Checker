// GrapheSync.cpp: implementation of the GrapheSync class.
//
//////////////////////////////////////////////////////////////////////


#include "GrapheSync.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GrapheSync::GrapheSync() {

}

GrapheSync::~GrapheSync() {

}

int GrapheSync::getNodesCount() {
    return m_noeuds.size();
}

int GrapheSync::getCountArcs() {
    long somme = 0;
    for (long i = 0; i < m_noeuds.size(); i++) {
        somme += m_noeuds[i]->getCountArcs();
    }
    return somme;
}


NodeSG *GrapheSync::getNode(const int index) {
    return m_noeuds[index];
}




