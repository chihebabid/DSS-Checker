// PileRed.cpp: implementation of the PileRed class.
//
//////////////////////////////////////////////////////////////////////


#include "PileRed.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PileRed::PileRed() {

}

PileRed::~PileRed() {

}

int PileRed::isCycle(Marking *marq) {
    int result = -1;
    for (int i = 0; i < (m_liste.size() - 1) && result == -1; i++) {
        if (m_liste.at(i).isExist(marq)) result = i;
    }
    return result;
}
