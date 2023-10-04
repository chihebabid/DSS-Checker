// Graphe.h: interface for the Graphe class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHE_H__C80662A1_B55B_4869_B09F_ED7F6D9764F2__INCLUDED_)
#define AFX_GRAPHE_H__C80662A1_B55B_4869_B09F_ED7F6D9764F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PetriNet.h"
#include "Arc.h"

class Graphe {
public:
    Graphe();

    vector<Marking> *getListMarq();

    void addMarquage(Marking *m);

    void addArc(Arc *arc);

    bool existMarquage(Marking *mar);

    virtual ~Graphe();

private:
    vector<Marking> *m_nodes;
    vector<Arc> *m_arcs;

};

#endif // !defined(AFX_GRAPHE_H__C80662A1_B55B_4869_B09F_ED7F6D9764F2__INCLUDED_)
