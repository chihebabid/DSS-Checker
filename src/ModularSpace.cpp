// ModularSpace.cpp: implementation of the ModularSpace class.
//
//////////////////////////////////////////////////////////////////////


#include "ModularPetriNet.h"
#include "ModularSpace.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ModularSpace::ModularSpace()
{
	m_graphe_sync=new GrapheSync();
}

ModularSpace::~ModularSpace()
{
	delete m_graphe_sync;
	for (int i=0;i<m_automates.size();i++)
		delete m_automates[i];
}

long ModularSpace::getModulesCount()
{
	return m_automates.size();
}

long ModularSpace::getTotalNodesCount()
{
	long sum=0;
	sum+=getNodesCount();
	sum+=m_graphe_sync->getNodesCount();
	return sum;
}

long ModularSpace::getNodesCount()
{
	long sum=0;
	for (int i=0;i<getModulesCount();i++)
		sum+=m_automates[i]->getCountNodes();
	return sum;
}

long ModularSpace::getLocalArcsCount()
{
	long sum=0;
	for (int i=0;i<getModulesCount();i++)
		sum+=m_automates[i]->getCountArcs();
	return sum;
}

long ModularSpace::getTotalArcs()
{
	long sum=0;
	sum+=getLocalArcsCount();
	sum+=m_graphe_sync->getCountArcs();
	return sum;
}

void ModularSpace::addGraphe(int index, Automata *automata)
{

	for (int i=0;i<automata->getCountNodes();i++) {
		m_automates[index]->addNode(automata->getNode(i));
	}
	for (int j=0;j<automata->getCountArcs();j++) {
		m_automates[index]->addArc(*automata->getArc(j));
	}
}
/////////////////////////////////////////////////
// Sp�cifier le nombre de modules
//////////////////////////////////////////////////
void ModularSpace::setNbModules(const int n)
{
	m_automates.resize(n);
	for (int i=0;i<n;i++)
		m_automates[i]=new Automata();

}
/////////////////////////////////////////////////////////
// Ins�rer le graphe de synchronisation
/////////////////////////////////////////////////////////
void ModularSpace::addGrapheSync(GrapheSync *graphe)
{
	m_graphe_sync=graphe;
}

Automata* ModularSpace::getEspaceLocal(const int index)
{
	return m_automates[index];
}



void ModularSpace::setParent(ModularPetriNet *petri)
{
	m_petri=petri;
}

void ModularSpace::display(int choix,int i)
{

	if (choix==1)
	{

		cout<<"\nInformations relatives au module "<<i;
		cout<<"\nListe de noeuds";
		for (int j=0;j<m_automates[i]->getCountNodes();j++) {
			ListMarquage* noeud=m_automates[i]->getNode(j);
			cout<<"\nNoeud n�"<<j<<" : ";
			cout<<getCorrespondant(noeud,i).c_str();


		}
		cout<<"\nListe des arcs";
		for (int j=0;j<m_automates[i]->getCountArcs();j++) {
			Arc* arc=m_automates[i]->getArc(j);
			cout<<"\nArc n�"<<j<<" : ";
			cout<<getCorrespondant(arc->getSource(),i).c_str()<<"="<<arc->getName().c_str()<<"=>"<<getCorrespondant(arc->getDestination(),i).c_str();
		}

	}
	int nb_arcs_locaux=0;
	for (int i=0;i<getModulesCount();i++)
	{
		nb_arcs_locaux+=m_automates[i]->getCountArcs();
	}

	if (choix==2)
	{
		cout<<"\nListe de noeuds du SG";
		for (int j=0;j<m_graphe_sync->getNodesCount();j++) {
		cout<<"\nNoeud n�"<<j<<" : ";
		cout<<getCorrespondantNodeSG(m_graphe_sync->getNode(j)).c_str();
		for (int k=0;k<m_graphe_sync->getNode(j)->m_arcs->size();k++)
		{
			cout<<"\n"<<
			"("<<getCorrespondantNodeSG(m_graphe_sync->getNode(j)->getArc(k)->getMarquageSource()).c_str();
			cout<<","<<m_graphe_sync->getNode(j)->getArc(k)->getFusion()->getName().c_str()<<",";
			//cout<<m_graphe_sync->getNode(j)->getArc(k)->getNodeSGDest();
			cout<<getCorrespondantNodeSG(m_graphe_sync->getNode(j)->getArc(k)->getNodeSGDest()).c_str()<<")";

		}
		}
	}
	if (choix==3)
	{
	cout<<"\n#arcs locaux :"<<nb_arcs_locaux;
	cout<<"\n#noeuds du SG:"<<m_graphe_sync->getNodesCount();
	cout<<"\n#arcs du SG : "<<m_graphe_sync->getCountArcs();
	cout<<"\n#noeuds locaux : "<<getNodesCount();
	cout<<"\nN#total de noeuds  : "<<getTotalNodesCount()<<"\n";
	}

}

string ModularSpace::getCorrespondant(ListMarquage *noeud,const int index_module)
{
	string resultat="";
	char chaine[100]="\0";

	for (int i=0;i<noeud->getCount();i++) {

		Marking m=*noeud->getMarquage(i);
		for (int j=0;j<m_petri->getModule(index_module)->getPlacesCount();j++) {
			if (m.get8BitsValue(j)!=0) {
				if (m.get8BitsValue(j)!=1) {
					sprintf(chaine,"%d\0",(int)m.get8BitsValue(j));
					resultat+=chaine;

				}
				resultat+=m_petri->getModule(index_module)->getPlace(j).getName();

			}
			cout<<"cc "<<m_petri->getModule(index_module)->getPlace(j).getName()<<endl;
		}

		resultat+=".";
	}
	return resultat;
}

void ModularSpace::setGrapheSync(GrapheSync *graphe)
{
	m_graphe_sync=graphe;

}

GrapheSync* ModularSpace::getGrapheSync()
{

	return m_graphe_sync;

}

string ModularSpace::getCorrespondantMarquage(Marking marq,const int index)
{
	string resultat="";
	char chaine[100]="\0";
	for (int j=0;j<m_petri->getModule(index)->getPlacesCount();j++) {
			if (marq.get8BitsValue(j)!=0) {
				if (marq.get8BitsValue(j)!=1) {
					sprintf(chaine,"%d\0",(int)marq.get8BitsValue(j));
					resultat+=chaine;
				}
				resultat+=m_petri->getModule(index)->getPlace(j).getName();
				resultat+=".";
			}

		}
	return resultat;
}

string ModularSpace::getCorrespondantNodeSG(NodeSGSimplified* node)
{
	string result="";
	for (int i=0;i<getModulesCount();i++) {

		if (node->getMarquage(i)->isVide()==true) result+="@"; else
		result+=getCorrespondantMarquage(*node->getMarquage(i),i);
		result+="|";
	}
	return result;
}

void ModularSpace::insertGraphe(const int index, Automata *automata)
{
	m_automates[index]=automata;
}


void ModularSpace::display2()
{
	for (int i=0;i<getModulesCount();i++) {
		cout<<"\nInformations relatives au module "<<i;
		cout<<"\nListe de noeuds";
		for (int j=0;j<m_automates[i]->getCountNodes();j++) {
			ListMarquage* noeud=m_automates[i]->getNode(j);
			cout<<"\nNoeud n�"<<j<<" : ";
//			cout<<getCorrespondant(noeud,i);


		}
	cout<<"\nListe des arcs";
		for (int j=0;j<m_automates[i]->getCountArcs();j++) {
			Arc* arc=m_automates[i]->getArc(j);
			cout<<"\nArc n�"<<j<<" : ";
		//	cout<<getCorrespondant(arc->getSource(),i)<<"="<<arc->getName()<<"=>"<<getCorrespondant(arc->getDestination(),i);
		}
	}
	cout<<"\nInformations relatives au graphe de synchronisation";
	cout<<"\nNombre de noeuds :"<<m_graphe_sync->getNodesCount();
	cout<<"\nNombre d'arcs :"<<m_graphe_sync->getCountArcs();
	cout<<"\nListe de noeuds";
	for (int j=0;j<m_graphe_sync->getNodesCount();j++) {
		cout<<"\nNoeud n�"<<j<<" : ";
		cout<<getCorrespondantNodeSG(m_graphe_sync->getNode(j)).c_str();
		for (int k=0;k<m_graphe_sync->getNode(j)->m_arcs->size();k++)
		{
			cout<<"\n"<<
			"("<<getCorrespondantNodeSG(m_graphe_sync->getNode(j)->getArc(k)->getMarquageSource()).c_str();
			cout<<","<<m_graphe_sync->getNode(j)->getArc(k)->getFusion()->getName().c_str()<<",";
			//cout<<m_graphe_sync->getNode(j)->getArc(k)->getNodeSGDest();
			cout<<getCorrespondantNodeSG(m_graphe_sync->getNode(j)->getArc(k)->getNodeSGDest()).c_str()<<")";

		}


	}


	cout<<"\nNombre des arc : "<<m_graphe_sync->getCountArcs();
	cout<<"\nNombre de module : "<<getModulesCount();
	cout<<"\nNombre de noeuds locaux : "<<getNodesCount();
	cout<<"\nNombre total de noeuds  : "<<getTotalNodesCount()<<"\n";
}
