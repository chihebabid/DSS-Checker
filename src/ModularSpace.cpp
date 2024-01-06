// ModularSpace.cpp: implementation of the ModularSpace class.
//
//////////////////////////////////////////////////////////////////////


#include "ModularPetriNet.h"
#include "ModularSpace.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ModularSpace::ModularSpace() {
    m_graphe_sync = new GrapheSync();
}

ModularSpace::~ModularSpace() {
    delete m_graphe_sync;
    for (int i = 0; i < m_automates.size(); i++)
        delete m_automates[i];
}

long ModularSpace::getModulesCount() {
    return m_automates.size();
}

long ModularSpace::getTotalNodesCount() {
    long sum = 0;
    sum += getNodesCount();
    sum += m_graphe_sync->getNodesCount();
    return sum;
}

long ModularSpace::getNodesCount() {
    long sum = 0;
    for (int i = 0; i < getModulesCount(); i++)
        sum += m_automates[i]->getCountNodes();
    return sum;
}

long ModularSpace::getLocalArcsCount() {
    long sum = 0;
    for (int i = 0; i < getModulesCount(); i++)
        sum += m_automates[i]->getCountArcs();
    return sum;
}


string ModularSpace::getCorrespondant(ListMarquage *noeud, const int index_module) {
    string resultat = "";
    char chaine[100] = "\0";

    for (int i = 0; i < noeud->getCount(); i++) {

        Marking m = *noeud->getMarquage(i);
        for (int j = 0; j < m_petri->getModule(index_module)->getPlacesCount(); j++) {
            if (m.get8BitsValue(j) != 0) {
                if (m.get8BitsValue(j) != 1) {
                    sprintf(chaine, "%d\0", (int) m.get8BitsValue(j));
                    resultat += chaine;

                }
                resultat += m_petri->getModule(index_module)->getPlace(j).getName();

            }
            cout << "cc " << m_petri->getModule(index_module)->getPlace(j).getName() << endl;
        }

        resultat += ".";
    }
    return resultat;
}


string ModularSpace::getCorrespondantMarquage(Marking marq, const int index) {
    string resultat = "";
    char chaine[100] = "\0";
    for (int j = 0; j < m_petri->getModule(index)->getPlacesCount(); j++) {
        if (marq.get8BitsValue(j) != 0) {
            if (marq.get8BitsValue(j) != 1) {
                sprintf(chaine, "%d\0", (int) marq.get8BitsValue(j));
                resultat += chaine;
            }
            resultat += m_petri->getModule(index)->getPlace(j).getName();
            resultat += ".";
        }
    }
    return resultat;
}

string ModularSpace::getCorrespondantNodeSG(NodeSGSimplified *node) {
    string result = "";
    for (int i = 0; i < getModulesCount(); i++) {
        if (node->getMarquage(i)->isVide() == true) result += "@";
        else
            result += getCorrespondantMarquage(*node->getMarquage(i), i);
        result += "|";
    }
    return result;
}
