// DisplaySummary.cpp: implementation of the DisplaySummary class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "DisplaySummary.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DisplaySummary::DisplaySummary() {

}

DisplaySummary::~DisplaySummary() {

}

void DisplaySummary::setPetr(CModularPetriNet *petri) {
    m_petri = petri;
}

void DisplaySummary::display() {
    printf("Nombre de module : %d\n", m_petri->getNbModules());
    printf("Nombre de transitions synchronis�es :%d\n", m_petri->getCountFusions());
    for (int i = 0; i < m_petri->getCountFusions(); i++)
        printf("%s (%d locales),", m_petri->getFusion(i)->getName().c_str(), m_petri->getFusion(i)->getCountTransitionsLocales());

    for (int i = 0; i < m_petri->getNbModules(); i++) {
        printf("\n=====> Module %d\n", i);
        printf("Nombre de places : %d\n", m_petri->getModule(i)->getPlacesCount());
        for (int j = 0; j < m_petri->getModule(i)->getPlacesCount(); j++)
            printf("%s (%d),", m_petri->getModule(i)->getPlace(j).getName().c_str(), m_petri->getModule(i)->getPlace(j).getTokens());
        printf("\n");
        printf("Nombre de transitions : %d\n", m_petri->getModule(i)->getTransitionsCount());
        for (int j = 0; j < m_petri->getModule(i)->getTransitionsCount(); j++)
            printf("%s, ", m_petri->getModule(i)->getTransition(j).getName().c_str());
    }

}
