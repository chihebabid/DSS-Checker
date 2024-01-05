// Operations.cpp: implementation of the Operations class.
//
//////////////////////////////////////////////////////////////////////


#include "Operations.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool Operations::find(vector<Marking> list, Marking *marq) {

    for (int i = 0; i < list.size(); i++)
        if (*marq == list.at(i)) return true;
    return false;
}



///////////////////////////////////////////////////////////////////////////////////
// V�rifier si toutes les transitons de 'liste' sont �tiquet�es par 'transition'
///////////////////////////////////////////////////////////////////////////////////
bool Operations::verifier(vector<Transition *> &liste, string transition) {
    int nombre = 0;
    for (int i = 0; i < liste.size(); i++)
        if (transition == liste.at(i)->getName()) nombre++;
    return (nombre == liste.size());
}


void Operations::reduceArcs(vector<ArcSG *> **list_arcs) {

    vector<ArcSG *> *temp = new vector<ArcSG *>();
    for (int i = 0; i < (*list_arcs)->size(); i++) {
        if (!findArc(temp, (*list_arcs)->at(i)))
            temp->push_back((*list_arcs)->at(i));
        else delete (*list_arcs)->at(i);
    }
    delete *list_arcs;
    *list_arcs = temp;

}

bool Operations::findArc(vector<ArcSG *> *liste, ArcSG *arc) {
    for (int i = 0; i < liste->size(); i++) {
        if (*liste->at(i) == *arc) return true;
    }
    return false;
}
