// Automata.cpp: implementation of the Automata class.
//
//////////////////////////////////////////////////////////////////////


#include "Automata.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Automata::Automata() {
    m_parent = nullptr;
}

Automata::~Automata() {
    for (unsigned int i = 0; i < m_nodes.size(); i++)
        delete m_nodes[i];
    m_nodes.clear();
    for (unsigned int i = 0; i < m_arcs.size(); i++)
        delete m_arcs[i];
    m_arcs.clear();
}

long Automata::getCountNodes() {
    return m_nodes.size();
}

long Automata::getCountArcs() {
    return m_arcs.size();
}

bool Automata::addNode(Noeud *noeud) {
    if (!isNodeInclude(*noeud)) {
        m_nodes.push_back(noeud);
        return true;
    }
    return false;
}

bool Automata::isNodeExist(Noeud node) {
    int indice = -1;
    for (unsigned int i = 0; i < m_nodes.size() && indice == -1; i++) {
        if (*m_nodes[i] == node) indice = i;
    }
    return !(indice == -1);
}


//////////////////////////////////////////
// Ajouter un arc dans le graphe
//////////////////////////////////////////
void Automata::addArc(ListMarquage *node_source, const string libelle_arc, ListMarquage *node_dest) {
    if (!isArcExist(node_source, libelle_arc, node_dest)) {
        Arc *arc = new Arc();
        arc->setSource(node_source);
        arc->setDestination(node_dest);
        arc->setName(libelle_arc);
        m_arcs.push_back(arc);
    }

}

void Automata::addArc(Marking source, const string libelle_arc, Marking dest) {
    vector<ListMarquage *> liste_source = getNodesContainMarquage(&source);
    vector<ListMarquage *> liste_dest = getNodesContainMarquage(&dest);
    for (int i = 0; i < liste_source.size(); i++)
        for (int j = 0; j < liste_dest.size(); j++)
            addArc(liste_source[i], libelle_arc, liste_dest[j]);

}

vector<ListMarquage *> Automata::getNodesContainMarquage(Marking *marq) {
    vector<ListMarquage *> liste;
    for (int i = 0; i < m_nodes.size(); i++) {
        ListMarquage *node = m_nodes.at(i);
        if (node->existMarquage(marq)) liste.push_back(node);
    }
    return liste;
}

bool Automata::isMarquageExist(Marking *marq) {

    for (int i = 0; i < m_nodes.size(); i++) {
        Noeud *node = m_nodes[i];
        if (node->existMarquage(marq)) return true;
    }
    return false;
}

ListMarqLoc Automata::getListMarquages() {
    ListMarqLoc liste;

    for (int i = 0; i < m_nodes.size(); i++) {
        for (int j = 0; j < m_nodes[i]->getCount(); j++) {
            int index = -1;
            for (int k = 0; k < liste.size() && index == -1; k++) {
                if (liste[k] == *m_nodes[i]->getMarquage(j)) index = k;

            }
            if (index == -1) {
                liste.push_back(*m_nodes[i]->getMarquage(j));
            }
        }
    }
    return liste;
}

Marking Automata::getRepresentativeNode() {
    return *m_nodes[0]->getMarquage(0);
}


ListMarquage *Automata::getNode(const int index) {
    return m_nodes[index];
}

Arc *Automata::getArc(const int index) {
    return m_arcs[index];
}

void Automata::addArc(Arc arc) {
    addArc(*arc.getSource()->getMarquage(0), arc.getName(), *arc.getDestination()->getMarquage(0));
}

///////////////////////////////////////
// V�rifier si un arc existe d�j�
///////////////////////////////////////
bool Automata::isArcExist(ListMarquage *source, string t, ListMarquage *destination) {
    bool res = false;
    for (int i = 0; i < m_arcs.size() && !res; i++) {
        Arc *arc = m_arcs[i];
        if (*arc->getSource() == *source &&
            *arc->getDestination() == *destination &&
            (arc->getName() == t))
            res = true;
    }
    return res;
}

////////////////////////////////////////////////
// Sp�cifier l'espace d'�tats modulaire parent
/////////////////////////////////////////////////
void Automata::setParent(ModularSpace *parent, const int num) {
    m_parent = parent;
    m_code = num;
}

////////////////////////////////////////////////////////////////////
// Affichage des informations relatives � cet espace d'�tats local
////////////////////////////////////////////////////////////////////
void Automata::display() {
    cout << "\nListe de marquages " << getNumero() << endl;
    for (int j = 0; j < getCountNodes(); j++) {
        ListMarquage *noeud = getNode(j);
        cout << "\nNoeud n°" << j << " : ";
        cout << m_parent->getCorrespondant(noeud, getNumero()).c_str();
    }
    cout << "\nListe des arcs";
    for (int j = 0; j < getCountArcs(); j++) {
        Arc *arc = getArc(j);
        cout << "\nArc n°" << j << " : ";
        cout << m_parent->getCorrespondant(arc->getSource(), getNumero()).c_str() << "=" << arc->getName().c_str() << "=>" << m_parent->getCorrespondant(arc->getDestination(), getNumero()).c_str();
    }
}

/***********************************************************/
void Automata::displayDSS() {


}


////////////////////////////////////////////////////
// Renvoyer le num�ro de cet espace d'�tats local
////////////////////////////////////////////////////
int Automata::getNumero() {
    return m_code;
}

////////////////////////////////////////////////////
// Sp�cifier le num�ro de cet espace d'�tats local
////////////////////////////////////////////////////
void Automata::setNumero(const int num) {
    m_code = num;
}


vector<ListMarquage *> Automata::getParents(ListMarquage *node) {
    vector<ListMarquage *> list;
    for (int i = 0; i < m_arcs.size(); i++) {
        if (m_arcs[i]->getDestination() == node)
            list.push_back(m_arcs[i]->getSource());
        list_transition.push_back(m_arcs[i]->getName());
    }
    return list;
}


void Automata::suppress(ListMarquage *noeud) {
    bool deleted = false;
    vector<ListMarquage *>::iterator Itor_compteur;
    int i = 0;
    for (Itor_compteur = m_nodes.begin(); Itor_compteur < m_nodes.end() && !deleted; Itor_compteur++) {
        if (*m_nodes.at(i) == *noeud) {
            m_nodes.erase(Itor_compteur, Itor_compteur + 1);
            deleted = true;
        }
        i++;
    }
}

bool Automata::isNodeInclude(Noeud &node) {
    bool result = false;
    for (int i = 0; i < m_nodes.size() && !result; i++) {
        result = true;
        for (int j = 0; j < node.getCount() && result; j++) {
            if (!m_nodes[i]->existMarquage(node.getMarquage(j))) result = false;
        }
    }
    return result;
}

void Automata::addArcs(ListMarquage *source, vector<Transition> &liste_transitions, ListMarquage *destination) {
    for (int i = 0; i < liste_transitions.size(); i++) {
        if (!isArcExist(source, liste_transitions.at(i).getName(), destination)) {
            Arc *arc = new Arc();
            arc->setSource(source);
            arc->setDestination(destination);
            arc->setName(liste_transitions.at(i).getName());
            m_arcs.push_back(arc);
        }
    }
}

////////////////////////////////////////////////////////////////
// Substituer une liste de noeuds par un seul noeud
////////////////////////////////////////////////////////////////
void Automata::substitute(vector<ListMarquage *> liste_noeuds, ListMarquage *noeud) {
    addNode(noeud);
    for (int i = 0; i < liste_noeuds.size(); i++) {
        suppress(liste_noeuds.at(i));
        // Mettre � jour les arcs
        for (int j = 0; j < m_arcs.size(); j++) {
            if (m_arcs.at(j)->getSource() == liste_noeuds.at(i)) m_arcs.at(j)->setSource(noeud);
            if (m_arcs.at(j)->getDestination() == liste_noeuds.at(i)) m_arcs.at(j)->setDestination(noeud);
        }

    }
    suppressDuplicates();
}

//////////////////////////////////////////////////
// Suppression des arcs dupliqu�s
//////////////////////////////////////////////////
void Automata::suppressDuplicates() {
    vector<Arc *>::iterator Itor_debut;
    vector<Arc *>::iterator Itor_index;
    int i,j ;
    for (Itor_debut = m_arcs.begin(), i = 0; Itor_debut < m_arcs.end(); Itor_debut++, i++) {
        j = i + 1;
        for (Itor_index = (Itor_debut + 1); Itor_index < m_arcs.end(); Itor_index++, j++) {
            if (**Itor_index == **Itor_debut) {
                m_arcs.erase(Itor_index, Itor_index + 1);
                Itor_index--;
            }
        }
    }
}

ListMarquage *Automata::getFirstNodeContainMarquage(Marking *marq) {
    for (int i = 0; i < m_nodes.size(); i++) {
        if (m_nodes[i]->existMarquage(marq)) return m_nodes[i];
    }
    return NULL;
}

ListMarquage *Automata::addNode2(ListMarquage *groupe) {
    ListMarquage *groupe_existant = getFirstNodeContainMarquage(groupe->getLastMarquage());
    if (groupe_existant != NULL) {
        groupe_existant->addGroupe(*groupe);
        for (int i = 0; i < m_arcs.size(); i++) {
            if (m_arcs.at(i)->getSource() == groupe) m_arcs.at(i)->setSource(groupe_existant);
            if (m_arcs.at(i)->getDestination() == groupe) m_arcs.at(i)->setDestination(groupe_existant);
        }
        return groupe_existant;
    }
    m_nodes.push_back(groupe);
    return groupe;
}

const ModularSpace *Automata::getParent() {
    return m_parent;
}

static ModularSpace *Automata::m_parent;
