// GrapheSync.h: interface for the GrapheSync class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHESYNC_H__6B4B8E62_CC8F_4CC3_9368_00E84748F21A__INCLUDED_)
#define AFX_GRAPHESYNC_H__6B4B8E62_CC8F_4CC3_9368_00E84748F21A__INCLUDED_

#include "ArcSG.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "NodeSG.h"
class NodeSG;
class GrapheSync
{
public:

	bool estRedondant(Marking* marquage,const int index);
	NodeSG* getNode(const int index);
	NodeSG* addNode(NodeSG* node);
	bool isNodeExist(NodeSG* marq);
	int getCountArcs();
	int getNodesCount();
	GrapheSync();
	virtual ~GrapheSync();

private:

	vector<NodeSG*> m_noeuds;
};

#endif // !defined(AFX_GRAPHESYNC_H__6B4B8E62_CC8F_4CC3_9368_00E84748F21A__INCLUDED_)
