// Automata.h: interface for the Automata class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTOMATA_H__937453EB_86E8_4040_AE59_7FF2AC562346__INCLUDED_)
#define AFX_AUTOMATA_H__937453EB_86E8_4040_AE59_7FF2AC562346__INCLUDED_

#include "Transition.h"
#include "Marking.h"    // Added by ClassView

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ListMarquage.h"    // Added by ClassView
#include "ModularSpace.h"

typedef vector<Marking> ListMarqLoc;

class ModularSpace;

class Automata {
public:
    void suppress(ListMarquage *noeud);

    ListMarquage *addNode2(ListMarquage *groupe);

    ListMarquage *getFirstNodeContainMarquage(Marking *marq);

    Marking getRepresentativeNode();

    void substitute(vector<ListMarquage *> listenoeuds, ListMarquage *noeud);

    void addArcs(ListMarquage *source, vector<Transition> &liste_transitions, ListMarquage *destination);

    void setNumero(const int num);

    int getNumero();

    void display();

    void setParent(ModularSpace *parent, const int num);

    bool isArcExist(ListMarquage *source, string t, ListMarquage *destination);

    void addArc(Arc arc);

    Arc *getArc(const int index);

    ListMarquage *getNode(const int index);

    ListMarqLoc getListMarquages();

    bool isMarquageExist(Marking *marq);

    vector<ListMarquage *> getNodesContainMarquage(Marking *marq);

    void addArc(Marking source, const string libelle_arc, Marking dest);

    void addArc(ListMarquage *marq1, const string trans, ListMarquage *marq2);

    bool isNodeExist(Noeud node);

    bool addNode(ListMarquage *marquage);

    long getCountArcs();

    long getCountNodes();

    Automata();

    virtual ~Automata();

    void displayDSS();

    static const ModularSpace *getParent();

private:
    static ModularSpace *m_parent;

    void suppressDuplicates();

    bool isNodeInclude(Noeud &node);

    vector<ListMarquage *> getParents(ListMarquage *node);

    int m_code;
    vector<ListMarquage *> m_nodes;
    vector<Arc *> m_arcs;
    vector<string> list_transition;
};

#endif // !defined(AFX_AUTOMATA_H__937453EB_86E8_4040_AE59_7FF2AC562346__INCLUDED_)
