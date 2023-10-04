// Operations.cpp: implementation of the Operations class.
//
//////////////////////////////////////////////////////////////////////


#include "Operations.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Operations::Operations() {

}

Operations::~Operations() {

}

//DEL bvec Operations::bvec_shl(bvec bv, int nb_bits)
//DEL {
//DEL 	bvec decaler=bvec_con(2,2);
//DEL 	bvec result=bvec_copy(bv);
//DEL 	for (int i=0;i<nb_bits;i++) {
//DEL 		bvec temp=bvec_copy(result);
//DEL 		result=bvec_mul(result,decaler);
//DEL 	}
//DEL
//DEL 	return result;
//DEL }

//DEL void Operations::printVec(bvec vec)
//DEL {
//DEL 	for (int i=0;i<vec.bitnum();i++) {
//DEL 		if (vec[i]==bdd_true()) cout<<1;
//DEL 		else cout<<0;
//DEL 	}
//DEL }

bool Operations::find(vector<Marking> list, Marking *marq) {

    for (int i = 0; i < list.size(); i++)
        if (*marq == list.at(i)) return true;
    return false;
}

bool Operations::find(vector<string> liste, string value) {
    for (int i = 0; i < liste.size(); i++)
        if (value == liste.at(i)) return true;
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
