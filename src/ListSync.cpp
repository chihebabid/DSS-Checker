// ListSync.cpp: implementation of the ListSync class.
//
//////////////////////////////////////////////////////////////////////


#include "ListSync.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ListSync::ListSync() {

}

ListSync::~ListSync() {

}

void ListSync::addTransition(Transition t, Marking marq) {
	bool exist = false;
	for (int i = 0; i < m_list_sync.size() && !exist; i++)
		if (m_list_sync[i].getName() == t.getName())
			exist = true;
	if (!exist) {
		m_list_sync.push_back(t);
		m_list_marq.push_back(marq);
	}
}

void ListSync::empty() {
	m_list_sync.clear();
	m_list_marq.clear();
}

int ListSync::getCount() {
	return m_list_sync.size();
}

ListSync& ListSync::operator =(const ListSync &liste) {
	for (int i = 0; i < liste.m_list_sync.size(); i++) {
		m_list_marq[i] = liste.m_list_marq[i];
		m_list_sync[i] = liste.m_list_sync[i];
	}
	return *this;
}

Marking ListSync::getMarquage(const int index) {
	return m_list_marq[index];
}

