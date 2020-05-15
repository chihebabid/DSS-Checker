// GrapheSync.cpp: implementation of the GrapheSync class.
//
//////////////////////////////////////////////////////////////////////


#include "GrapheSync.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GrapheSync::GrapheSync()
{

}

GrapheSync::~GrapheSync()
{

}

int GrapheSync::getNodesCount()
{
	return m_noeuds.size();
}

int GrapheSync::getCountArcs()
{
	long somme=0;
	for (long i=0;i<m_noeuds.size();i++)
	{
		somme+=m_noeuds[i]->getCountArcs();
	}
	return somme;
}





bool GrapheSync::isNodeExist(NodeSG* marq)
{	
	for (int i=0;i<m_noeuds.size();i++) 
		if (*marq==*m_noeuds[i])
		{
			*marq=*m_noeuds[i];
			return true;
		}
	return false;
}

NodeSG* GrapheSync::addNode(NodeSG* node)
{
	for (int i=0;i<m_noeuds.size();i++) 
		if (*node==*m_noeuds[i])
		{			
			return m_noeuds[i];
		}
	m_noeuds.push_back(node);
	return node;	
}

NodeSG* GrapheSync::getNode(const int index)
{
	return m_noeuds[index];
}



bool GrapheSync::estRedondant(Marking *marquage,const int index)
{		
	for (int i=0;i<m_noeuds.size();i++)
	{
		NodeSG* node=m_noeuds.at(i);
		for (int j=0;j<node->getCountArcs();j++)
		{	
			if (node->getArc(j)->getMarquageSource()->getMarquage(index)->isVide()) ;
		    else
			if (*(node->getArc(j)->getMarquageSource()->getMarquage(index))==*marquage) return false;
		}
	}
	return true;
}



