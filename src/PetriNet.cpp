// PetriNet.cpp: implementation of the PetriNet class.
//
//////////////////////////////////////////////////////////////////////

#include "Automata.h"
#include "PetriNet.h"
#include "PileRed.h"
#include "PilePhase1.h"
#include "Element.h"
#include "PElement.h"
typedef vector<Element> Pile;
typedef vector<PElement> PStack;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
PetriNet::PetriNet()
{
	m_places.clear();
	m_transitions.clear();
	m_graphe=NULL;
}

PetriNet::~PetriNet()
{

}
//////////////////////////////////
// Renvoyer le nombre de places
//////////////////////////////////
int PetriNet::getPlacesCount()
{
	return m_places.size();
}

int PetriNet::getTransitionsCount()
{
	return m_transitions.size();
}
////////////////////////////////////////////////////
// Retourner le nombre de transitions franchissables
////////////////////////////////////////////////////
int PetriNet::getNbTransitionsFranchissables()
{
	int count=0;
	for (unsigned int i=0;i<m_transitions.size();i++)
		if (m_transitions.at(i).isFranchissable()) count++;
	return count;
}

//////////////////////////////////
// Ajouter une place
//////////////////////////////////
void PetriNet::addPlace(Place place)
{
	m_places.push_back(place);
}

//////////////////////////////////
// Ins�rer une transition
//////////////////////////////////
void PetriNet::addTransition(Transition transition)
{
	transition.setPetri(this->getNumero());
	m_transitions.push_back(transition);
}

//////////////////////////////////
// Ajouter une liste de places
//////////////////////////////////
void PetriNet::addListPlaces(vector<Place> liste_places)
{
	for (int i=0;i<liste_places.size();i++)
		m_places.push_back(liste_places[i]);
}

void PetriNet::addListTransitions(vector<Transition> liste_transitions)
{
	for (unsigned int i=0;i<liste_transitions.size();i++) {
		liste_transitions[i].setPetri(this->getNumero());
		m_transitions.push_back(liste_transitions[i]);
	}
}



////////////////////////////////////////////
//  Retourner l'a&dresse d'une place
////////////////////////////////////////////
Place* PetriNet::getPlaceAdresse(string placename)
{
	int indice=-1;
	for (int i=0;i<m_places.size() && indice==-1;i++)
		if (placename==m_places[i].getName()) indice=i;
	if (indice==-1) printf("Place introuvable : libell� d'une place in�xistante!\n");
	return &m_places[indice];
}

///////////////////////////////////////////////////////////////////////////////
// Retourner un pointeur sur une transition ayant le code pass� en param�tre //
///////////////////////////////////////////////////////////////////////////////
Transition* PetriNet::getTransitionAdresse(const int code)
{
	int indice=-1;
	for (int i=0;i<m_transitions.size() && indice==-1;i++) {
		if (code==m_transitions[i].getCode()) indice=i;
	}
	return &m_transitions[indice];
}

Place PetriNet::getPlace(const int index)
{
	return m_places[index];
}

Transition PetriNet::getTransition(const int index)
{
	return m_transitions[index];
}

////////////////////////////////
// Renvoyer le marquage courant
////////////////////////////////
Marking PetriNet::getMarquage()
{
	Marking vecteur;
	for (int i=0;i<m_places.size();i++) {
		vecteur.add8BitsValue(m_places[i].getTokens());
	}

	return vecteur;
}

void PetriNet::setMarquage(Marking* marquage)
{

	for (int i=0;i<m_places.size();i++) {
		m_places[i].setTokens(marquage->get8BitsValue(i));
	}
}
///////////////////////////////////////////////////////////////
// Renvoyer les libell�s des transitions franchissables
///////////////////////////////////////////////////////////////
vector<Transition*> PetriNet::getListeTransitionsFranchissables()
{
	vector<Transition*> liste_transitions;
	for (int i=0;i<m_transitions.size();i++)
		if (m_transitions.at(i).isFranchissable())
		{
			liste_transitions.push_back(&m_transitions.at(i));
		}

	return liste_transitions;
}

void PetriNet::tirer(Transition& t)
{
	Transition* transition=getTransitionAdresse(t.getCode());
	if (transition->isFranchissable()) transition->tirer();
}



void PetriNet::printMarquage()
{
	for (int i=0;i<m_places.size();i++) {
//		cout<<m_places[i].getName()<<"("<<m_places[i].getTokens()<<")";
		if (i!=m_places.size()-1) cout<<", ";
	}
}

int PetriNet::addPlacesEntrees(string nom_transition, vector<string> liste_places_entrees,vector<int> liste_poids)
{
	// Localisation de l'indice de la transition
	int indice=-1;
	for (int i=0;i<m_transitions.size() && indice==-1;i++) {
		if (m_transitions[i].getName()==nom_transition)
			indice=i;
	}
	if (indice!=-1) {
		// Ajout de places d'entr�e
		//printf("Places d'entr�e\n");
		for (int i=0;i<liste_places_entrees.size();i++) {
			m_transitions[indice].addPlaceEntree(getPlaceAdresse(liste_places_entrees[i]),liste_poids.at(i));
			//printf("%s, ",liste_places_entrees[i].mot.c_str());

		}
		return 0;
	}
	cout<<"\nErreur d'ajout d'une relation!\n";
	return -1;
}
///////////////////////////////////////////////////////////
// Ajouter les places d'entr�es � une transition
///////////////////////////////////////////////////////////
int PetriNet::addPlacesSorties(string nom_transition, vector<string> liste_places_sorties,vector<int> liste_poids)
{
	// Localisation de l'indice de la transition
	int indice=-1;
	for (int i=0;i<m_transitions.size() && indice==-1;i++) {
		if (m_transitions[i].getName()==nom_transition)
			indice=i;
	}
	if (indice!=-1) {
		// Ajout de places de sortie
		for (int j=0;j<liste_places_sorties.size();j++) {
			m_transitions[indice].addPlaceSortie(getPlaceAdresse(liste_places_sorties.at(j)),liste_poids.at(j));
			//printf("%s, ",liste_places_sorties[j].mot.c_str());
		}
		return 0;
	}
	printf("ERREUR : relation erron�e!\n");
	return -1;
}


////////////////////////////////////////////////////////////////////
/// Construire le graphe local accessible � partir d'un marquage ///
////////////////////////////////////////////////////////////////////
Automata* PetriNet::getLocalSpace(Marking marquage)
{
	vector<Marking> list_marq_inserted;
	Pile pile;
	Element elt;
	Automata* graphe=new Automata();
	elt.marquage=marquage;
	setMarquage(&marquage);
	elt.liste_transitions=getListeTransitionsFranchissables();
	pile.push_back(elt);

	ListMarquage* node=new ListMarquage();
	node->addMarquage(&marquage);
	// Indiquer si le marquage en question est r�cemment ins�r�
	if (!graphe->isMarquageExist(&marquage)) list_marq_inserted.push_back(marquage);
	graphe->addNode(node);
	while (pile.size()>0) {
		Element current_elt=pile.back();
		pile.pop_back();

		if (current_elt.liste_transitions.size()>0) {

			Transition* transition=current_elt.liste_transitions[current_elt.liste_transitions.size()-1];
			current_elt.liste_transitions.pop_back();

			// On doit tirer la transition correspondante apr�s avoir pr�ciser le marquage
			setMarquage(&current_elt.marquage);
			tirer(*transition);

			node=new ListMarquage();
			Marking m=getMarquage();
			node->addMarquage(&m);
			// Indiquer si le marquage en question est r�cemment ins�r�
			if (!graphe->isMarquageExist(&marquage)) list_marq_inserted.push_back(getMarquage());
			//cout<<"\n The new  marquage="<<getMarquage().getString();
			bool result=graphe->addNode(node);
			//arc=new Arc();

			graphe->addArc(current_elt.marquage,transition->getName(),*node->getMarquage(0));
			//cout<<"\n Ajout du noeud r�usssi ="<<result;
			if (current_elt.liste_transitions.size()>0) pile.push_back(current_elt);
			// Ajouter le marquage dans la pile et ses transitions franchissables
			if (result) {
				elt.marquage=getMarquage();
				setMarquage(&elt.marquage);
				elt.liste_transitions=getListeTransitionsFranchissables();
				if (elt.liste_transitions.size()>0) pile.push_back(elt);
			}
		}
	}
	return graphe;
}


/////////////////////////////////////
/// Renvoyer le nom d'un marquage ///
/////////////////////////////////////
string PetriNet::getMarquageName(Marking marquage)
{
	string resultat="";
	char chaine[100]="\0";
	for (int j=0;j<getPlacesCount();j++) {
		if (marquage.get8BitsValue(j)!=0) {
			if (marquage.get8BitsValue(j)!=1) {
				sprintf(chaine,"%d\0",(int)marquage.get8BitsValue(j));
				resultat+=chaine;
			}
			resultat+=getPlace(j).getName();
			resultat+="_";
		}
	}
	return resultat;
}
///////////////////////////////////////////////////////////////////////////
/// Construire le graphe local accessible r�duit � partir d'un marquage ///
///////////////////////////////////////////////////////////////////////////
Automata* PetriNet::getLocalSpaceReduced(vector<Marking>& liste_marquages,GrapheSync* graphe_sync, Automata* graphe)
{
	//cout<<"\n-----------------Transition synchronis�e est franchie-----------------\n";
	m_graphe=graphe;

	PileRed pile;
	ElementRed elt;
	for (int i=0;i<m_graphe->getCountNodes();i++)
	{
			ListMarquage* node;
			node=m_graphe->getNode(i);
			elt.groupe=node;
			elt.liste_fils=getListFilsEx(node);
			elt.etat=false;
			if (elt.liste_fils.size()>0) pile.m_liste.push_back(elt);

	}

	for (int  i=0;i<liste_marquages.size();i++)
	{
		if (!m_graphe->isMarquageExist(&liste_marquages.at(i)))
		{
			ListMarquage* groupe;
			groupe=new ListMarquage();

			groupe->addMarquage(&liste_marquages.at(i));
			elt.groupe=groupe;
			elt.liste_fils=getListeFils(liste_marquages.at(i));
			elt.etat=estLocalementRedondant(&liste_marquages.at(i)) && graphe_sync->estRedondant(&liste_marquages.at(i),getNumero());
			pile.m_liste.push_back(elt);
		}
	}

	vector<ListMarquage*> liste_groupes;


	while (pile.m_liste.size()>0) {

		ElementRed current_elt=pile.m_liste.back();
		pile.m_liste.pop_back();

		/*cout<<"\n"<<m_graphe->m_parent->getCorrespondant(current_elt.groupe,getNumero()).c_str()<<" "<<current_elt.liste_fils.size()<<" "<<current_elt.etat;
		for (int k=0;k<current_elt.liste_fils.size();k++)
		cout<<"\n Fils "<<k<<" : "<<m_graphe->m_parent->getCorrespondantMarquage(current_elt.liste_fils.at(k).getMarquage(),getNumero()).c_str();*/

		  //int d;cin>>d;
		/**************************************************/
		/** Si l'�l�ment courant n'est pas tau-redondant **/
		/**************************************************/

			if (current_elt.etat==false)
			{

				if (current_elt.liste_fils.size()>0)
				{
					// R�cuprer un fils de ce marquage
					Fils fils=current_elt.liste_fils.back();
					current_elt.liste_fils.pop_back();
					pile.m_liste.push_back(current_elt);

					if (!graphe->isMarquageExist(&fils.getMarquage()))
					{
						/*test++;
						cout<<"\nNoeud ajoute : "<<m_graphe->m_parent->getCorrespondant(current_elt.groupe,getNumero()).c_str();
						cout<<"\n--------------------------------------------------------------"<<test;*/
						// Enregistrer le marquage fils dans la pile et ses fils
						m_graphe->addNode2(current_elt.groupe);
						elt.groupe=new ListMarquage();
						elt.groupe->addMarquage(&fils.getMarquage());
						elt.liste_fils=getListeFils(fils.getMarquage());
						elt.etat=(estLocalementRedondant(&fils.getMarquage()) && graphe_sync->estRedondant(&fils.getMarquage(),getNumero()));
						pile.m_liste.push_back(elt);
						m_graphe->addArcs(current_elt.groupe,fils.getListeTransitions(),elt.groupe);

					}
					else
					{
						liste_groupes=graphe->getNodesContainMarquage(&fils.getMarquage());
						for (unsigned int index=0;index<liste_groupes.size();index++)
						{

							m_graphe->addArcs(current_elt.groupe,fils.getListeTransitions(),liste_groupes.at(index));
						}
					}
				}
				else
				{
					/*test++;
					cout<<"\nNoeud ajoute : "<<m_graphe->m_parent->getCorrespondant(current_elt.groupe,getNumero()).c_str();
					cout<<"\n--------------------------------------------------------------"<<test;*/
					m_graphe->addNode2(current_elt.groupe);

				}

			}
			/********************************************/
			/** Si l'�l�ment courant est tau-redondant **/
			/********************************************/
			else {

				if (current_elt.liste_fils.size()>0)
				{

					// On commence par r�cup�rer un de ses fils
					Fils fils=current_elt.liste_fils.back();
					Marking marquage_fils=fils.getMarquage();


					current_elt.liste_fils.pop_back();
					pile.m_liste.push_back(current_elt);

					// Collecter les informations relatives au fils
					if (!graphe->isMarquageExist(&marquage_fils))
					{

						elt.groupe=new ListMarquage();
						*elt.groupe=*current_elt.groupe;
						elt.groupe->addMarquage(&marquage_fils);
						elt.liste_fils=getListeFils(marquage_fils);
						elt.etat=(estLocalementRedondant(&marquage_fils) && graphe_sync->estRedondant(&marquage_fils,getNumero()));
						pile.m_liste.push_back(elt);

					}
					else
					{
						liste_groupes=graphe->getNodesContainMarquage(&fils.getMarquage());
						for (int index=0;index<liste_groupes.size();index++)
						{
							liste_groupes.at(index)->addGroupe(*current_elt.groupe);
							//m_graphe->addArcs(current_elt.groupe,fils.getListeTransitions(),liste_groupes.at(index));
						}
					}
				}
			}


	}
	/*cout<<"\ntest :"<<test;
	cout<<"\n#nodes : "<<graphe->getCountNodes();	*/
	//exit(0);
	graphe->display();
	return graphe;
}
/////////////////////////////////////////////////////////////////////////////////////
// Renvoyer la liste des marquages locaux accessibles � partir d'un marquage donn�e
/////////////////////////////////////////////////////////////////////////////////////
vector<Marking>* PetriNet::getListMarquageAccFrom(Marking marquage,Automata *automata,ListMarquage** liste)
{
	vector<Marking>* m_list_marq_inserted=new vector<Marking>();
	m_list_marq_inserted->clear();
	m_graphe=automata;
	m_list_marq_inserted->push_back(marquage);
	PilePhase1 pile;
	ElementPhase1 elt;
	ListMarquage* groupe=new ListMarquage();
	groupe->addMarquage(&marquage);
	elt.groupe=groupe;
	elt.liste_fils=getListeFilsMarquages(marquage);
	pile.m_liste.push_back(elt);


	while (pile.m_liste.size()>0) {

		ElementPhase1 current_elt=pile.m_liste.back();
		pile.m_liste.pop_back();
		if (current_elt.liste_fils.getCount()>0)
		{

			Marking marquage=current_elt.liste_fils.pop();
			pile.m_liste.push_back(current_elt);
			//Lorsqu'il existe une CFC
			int index;
			if ((index=pile.isCycle(&marquage))!=-1)
			{
				replaceCyclePhase1(&pile,index,&marquage);
			}
			//Sinon ajouter le marquage courant et ces fils
			else
			{
				if (m_list_marq_inserted->end()==find(m_list_marq_inserted->begin(),m_list_marq_inserted->end(),marquage))
				{
					m_list_marq_inserted->push_back(marquage);
					elt.groupe=new ListMarquage();
					elt.groupe->addMarquage(&marquage);
					elt.liste_fils=getListeFilsMarquages(marquage);
					pile.m_liste.push_back(elt);
				}
			}
		}
		else
		{
			if (current_elt.etat==true)
			{
				addArcs(current_elt.groupe,m_graphe->addNode2(current_elt.groupe));

			}
			else {
				delete current_elt.groupe;
			}
		}

	}
	// D�terminer le noeud initial
	*liste=automata->getFirstNodeContainMarquage(&m_list_marq_inserted->at(0));
	return m_list_marq_inserted;
}
///////////////////////////////////
// Sp�cifier le num�ro du module //
///////////////////////////////////
void PetriNet::setNumero(const int index)
{
	m_numero=index;
}

/////////////////////////////////////
// Renvoyer le num�ro de ce module //
/////////////////////////////////////
int PetriNet::getNumero()
{
	return m_numero;
}

//////////////////////////////
// Renommer les transitions //
//////////////////////////////
void PetriNet::renommerTransitions(vectorString transitions)
{
	for (int i=0;i<m_transitions.size();i++) {
		if (!m_transitions.at(i).isSync()) {
			if (!Operations::find(transitions,m_transitions.at(i).getName()))
				m_transitions.at(i).setName("$");
		}
	}
}

/////////////////////////////////////////////////////////////////////
// Renvoyer un pointeur vers une transition en connaissant son nom //
/////////////////////////////////////////////////////////////////////
Transition* PetriNet::getTransitionAdresse(const string nom_transition)
{
	int indice=-1;
	for (int i=0;i<m_transitions.size() && indice==-1;i++) {
		if (nom_transition==m_transitions[i].getName()) indice=i;
	}
	if (indice!=-1)
		return &m_transitions[indice];
	else return NULL;
}

vector<Fils> PetriNet::getListeFils()
{
	return getListeFils(getMarquage());
}

////////////////////////////////////////////////////
// Tester si un marquage est localement redondant //
////////////////////////////////////////////////////
bool PetriNet::estLocalementRedondant(Marking *marq)
{
	setMarquage(marq);
	vector<Transition*> liste_transition=getListeTransitionsFranchissables();
	if (liste_transition.size()==0)
	{
		return false;
	}
	return (Operations::verifier(liste_transition,"$"));

}

////////////////////////////////////////////////////////////
// Mettre � jour la liste des fils
////////////////////////////////////////////////////////////
void PetriNet::update(vector<Fils>& liste_fils, Marking marq,Transition& transition)
{
	int indice=-1;
	for (int i=0;i<liste_fils.size() && (indice==-1);i++)
	{
		if (liste_fils.at(i).getMarquage()==marq) indice=i;
	}
	if (indice!=-1) {
		liste_fils.at(indice).addTransition(transition);
	}
	else {
		Fils fils;
		fils.setMarquage(marq);
		fils.addTransition(transition);
		liste_fils.push_back(fils);
	}
}
////////////////////////////////////////////////////
// Renvoyer la liste des marquages fils d'un marquage donn�
////////////////////////////////////////////////////
ListMarquage PetriNet::getListeFilsMarquages(Marking& marq)
{
	setMarquage(&marq);
	ListMarquage result;
	vector<Transition*> liste_transitions=getListeTransitionsFranchissables();
	for (int i=0;i<liste_transitions.size();i++) {
		setMarquage(&marq);
		tirer(*liste_transitions.at(i));
		Marking m=getMarquage();
		result.addMarquage(&m);
	}
	return result;
}
///////////////////////////////////////////////////////////////////
// Remplacer les noeuds apparetnant � un cycle par un seul noeud //
///////////////////////////////////////////////////////////////////
void PetriNet::replaceCyclePhase1(PilePhase1 *pile, const int index,Marking* marq)
{
	ElementPhase1 nouveau_elt;
	nouveau_elt.groupe=new ListMarquage();
	nouveau_elt.etat=true;
	for (int i=pile->m_liste.size()-1;i>=index;i--)
	{
		ElementPhase1 elt=pile->m_liste.back();
		nouveau_elt.groupe->addGroupe(*elt.groupe);
		nouveau_elt.liste_fils.addGroupe(elt.liste_fils);
		pile->m_liste.pop_back();
	}
	nouveau_elt.groupe->addMarquage(marq);
	//nouveau_elt.liste_fils.addGroupe(getListeFilsMarquages(*marq));
	pile->m_liste.push_back(nouveau_elt);
}
///////////////////////////////////////////////////////////////////
// Remplacer les noeuds apparetnant � un cycle par un seul noeud //
///////////////////////////////////////////////////////////////////
void PetriNet::replaceCycle(PileRed *pile, const int index, Fils *fils)
{
	vector<ListMarquage*> liste_noeuds_supprimer;
	ElementRed nouveau_elt;
	ListMarquage* nouveau_groupe=new ListMarquage();
	nouveau_groupe->addMarquage(&fils->getMarquage());
	for (int i=pile->m_liste.size()-1;i>=index;i--) {

		ElementRed elt=pile->m_liste.back();
		nouveau_elt.addFils(elt.liste_fils);

		liste_noeuds_supprimer.push_back(elt.groupe);
		nouveau_groupe->addGroupe(*elt.groupe);
		pile->m_liste.pop_back();

	}
	nouveau_elt.groupe=nouveau_groupe;
	nouveau_elt.etat=false;
	pile->m_liste.push_back(nouveau_elt);
	m_graphe->addArcs(nouveau_groupe,fils->getListeTransitions(),nouveau_groupe);
	m_graphe->substitute(liste_noeuds_supprimer,nouveau_groupe);
}

bool PetriNet::aciveFusion(Marking *marq)
{
	setMarquage(marq);
	for (int i=0;i<m_transitions.size();i++)
	{
		if (m_transitions.at(i).isSync() && m_transitions.at(i).isFranchissableLocal()) return true;
	}
	return false;
}
////////////////////////////////////////////////////
// Renvoyer la liste des fils  d'un ensemble de marquages
////////////////////////////////////////////////////
vector<Fils> PetriNet::getListFils(ListMarquage *groupe)
{
	vector<Transition*> liste_transitions;
	vector<Fils> result;
	for (int i=0;i<groupe->getCount();i++)
	{
		setMarquage(groupe->getMarquage(i));
		liste_transitions=getListeTransitionsFranchissables();
		for (int t=0;t<liste_transitions.size();t++)
		{
			setMarquage(groupe->getMarquage(i));
			tirer(*liste_transitions.at(t));
			update(result,getMarquage(),*liste_transitions.at(t));
		}
	}
	return result;
}

////////////////////////////////////////////////////
// Renvoyer la liste des fils d'un marquage donn�
////////////////////////////////////////////////////
vector<Fils> PetriNet::getListeFils(Marking marq)
{
	setMarquage(&marq);
	vector<Fils> result;
	vector<Transition*> liste_transitions=getListeTransitionsFranchissables();
	for (int i=0;i<liste_transitions.size();i++) {
		setMarquage(&marq);
		tirer(*liste_transitions.at(i));
		update(result,getMarquage(),*liste_transitions.at(i));
	}
	return result;
}

////////////////////////////////////////////////////
// Renvoyer la liste des fils  d'un ensemble de marquages qui ne sont pas inclus dans groupe
////////////////////////////////////////////////////
vector<Fils> PetriNet::getListFilsEx(ListMarquage *groupe)
{
	vector<Transition*> liste_transitions;
	vector<Fils> result;
	for (int i=0;i<groupe->getCount();i++)
	{
		setMarquage(groupe->getMarquage(i));
		liste_transitions=getListeTransitionsFranchissables();

		for (int t=0;t<liste_transitions.size();t++)
		{
			setMarquage(groupe->getMarquage(i));
			tirer(*liste_transitions.at(t));
			Marking m=getMarquage();
			if (groupe->existMarquage(&m))
			{
				update(result,getMarquage(),*liste_transitions.at(t));
			}
		}
	}
	return result;
}



 bool PetriNet::isDivergent(Marking &marq)
{
Noeud* noeud=m_graphe->getFirstNodeContainMarquage(&marq);
	if (noeud==NULL) return false;
	vector<Transition*> liste_transitions;
	setMarquage(&marq);
	liste_transitions=getListeTransitionsFranchissables();
	for (int t=0;t<liste_transitions.size() ;t++)
		{
			setMarquage(&marq);
			tirer(*liste_transitions.at(t));
			Marking m=getMarquage();
			if (noeud->existMarquage(&m)) return true;
		}
	return false;

}

void PetriNet::addArcs(ListMarquage *groupe, ListMarquage *noeud)
{
	vector<Transition*> liste_transitions;

	for (int i=0;i<groupe->getCount();i++)
	{
		setMarquage(groupe->getMarquage(i));
		liste_transitions=getListeTransitionsFranchissables();

		for (int t=0;t<liste_transitions.size();t++)
		{
			setMarquage(groupe->getMarquage(i));
			tirer(*liste_transitions.at(t));
			Marking m=getMarquage();
			if (groupe->existMarquage(&m))
			{
				m_graphe->addArc(noeud,liste_transitions.at(t)->getName(),noeud);
			}
		}
	}

}




////////////////////////////////////////////////////////////////////
/// Construire le graphe local accessible � partir d'un marquage ///
////////////////////////////////////////////////////////////////////
Graphe* PetriNet::getLocalGraph(Marking marquage)
{
	vector<Marking> list_marq_inserted;
	Pile pile;
	Element elt;
	Graphe* graphe=new Graphe();
	elt.marquage=marquage;
	setMarquage(&marquage);
	elt.liste_transitions=getListeTransitionsFranchissables();
	pile.push_back(elt);

	ListMarquage* node=new ListMarquage();
	node->addMarquage(&marquage);
	// Indiquer si le marquage en question est r�cemment ins�r�
	if (!graphe->existMarquage(&marquage)) list_marq_inserted.push_back(marquage);
	graphe->addMarquage(&marquage);
	while (pile.size()>0) {
		Element current_elt=pile.back();
		pile.pop_back();

		if (current_elt.liste_transitions.size()>0) {

			Transition* transition=current_elt.liste_transitions[current_elt.liste_transitions.size()-1];
			current_elt.liste_transitions.pop_back();

			// On doit tirer la transition correspondante apr�s avoir pr�ciser le marquage
			setMarquage(&current_elt.marquage);
			tirer(*transition); // Franchissement

			Marking m=getMarquage();

			// Indiquer si le marquage en question est r�cemment inséré
			if (!graphe->existMarquage(&marquage)) list_marq_inserted.push_back(m);
			//cout<<"\n The new  marquage="<<getMarquage().getString();
			graphe->addMarquage(&m);
			//arc=new Arc();

			//graphe->addArc(current_elt.marquage,transition->getName(),*node->getMarquage(0));
			//cout<<"\n Ajout du noeud r�usssi ="<<result;
			if (current_elt.liste_transitions.size()>0) pile.push_back(current_elt);
			// Ajouter le marquage dans la pile et ses transitions franchissables
/*			if (result) {
				elt.marquage=getMarquage();
				setMarquage(&elt.marquage);
				elt.liste_transitions=getListeTransitionsFranchissables();
				if (elt.liste_transitions.size()>0) pile.push_back(elt);
			}*/
		}
	}
	return graphe;
}

// Construction de Meta-graphe

StateGraph* PetriNet::getStateGraph(Marking marquage)
{
	//vector<Marquage> list_marq_inserted;
	PStack pstack;
	PElement elt;
	StateGraph* state_graph=new StateGraph();
	elt.marquage=new Marking(marquage);
	setMarquage(&marquage);
	elt.liste_transitions=getListeTransitionsFranchissables();
	pstack.push_back(elt);

	// ListMarquage* node=new ListMarquage();
	// node->addMarquage(&marquage);
	// Indiquer si le marquage en question est r�cemment ins�r�
	//if (!state_graph->existState(&marquage)) list_marq_inserted.push_back(marquage);
	state_graph->addMarquage(elt.marquage);

	while (pstack.size()>0) {
		PElement current_elt=pstack.back();
		pstack.pop_back();

		while (current_elt.liste_transitions.size()>0) {

			Transition* transition=current_elt.liste_transitions.back();
			current_elt.liste_transitions.pop_back();

			// On doit tirer la transition correspondante apr�s avoir pr�ciser le marquage
			setMarquage(current_elt.marquage);
			//cout<<"\n The old  marquage="<<getMarquageName(getMarquage())<<endl;
			tirer(*transition); // Franchissement

			Marking* new_state;
			new_state=new Marking(getMarquage());
			// Indiquer si le marquage en question est r�cemment inséré
			//if (!state_graph->existState(new_state)) list_marq_inserted.push_back(*new_state);
			//cout<<"\n The old  marquage="<<current_elt.marquage.get8BitsValue()<<endl;
			//cout<<"firing of "<<(*transition).getName()<<endl;
			//cout<<"\n The new  marquage="<<getMarquageName(*new_state)<<endl;
			Marking* old_state=state_graph->addMarquage(new_state);
			//if (old_state) cout<<"\n The found  marquage="<<getMarquageName(*old_state)<<endl;
			
            //cout<<"\n The current  marquage (old)="<<getMarquageName(*current_elt.marquage)<<endl;
            if (!old_state) current_elt.marquage->addSucc(transition,new_state); 
            else {
                delete new_state;
                new_state=NULL;
                current_elt.marquage->addSucc(transition,old_state); 
                
            }
            
			
			//cout<<"\n Ajout du noeud réusssi ="<<result;
			//if (current_elt.liste_transitions.size()>0) pstack.push_back(current_elt);
			// Ajouter le marquage dans la pile et ses transitions franchissables
			if (!old_state) {
				elt.marquage=new_state;
				setMarquage(elt.marquage);
				elt.liste_transitions=getListeTransitionsFranchissables();
				if (elt.liste_transitions.size()>0) pstack.push_back(elt);
			}
		}
	}

	state_graph->computeSCCTarjan();
	return state_graph;
}


// Display Information related to a Meta-State

void PetriNet::printMetaState(MetaState *ms) {
    vector<Marking *>* list_states=ms->getListMarq();
    cout<<"Number of states: "<<list_states->size()<<endl;
    for (int i=0;i<list_states->size();i++) {
        cout<<"State n°"<<i<<": "<<getMarquageName(*list_states->at(i))<<endl;
    }

     vector<SCC*>* list_sccs=ms->getListSCCs();
    cout<<"Number of SCCs: "<<list_sccs->size()<<endl;
    for (int i=0;i<list_sccs->size();i++) {
        cout<<"SCC n°"<<i<<endl;
        vector<Marking*>* lscc_states=list_sccs->at(i)->getListStates();
        for (int j=0;j<lscc_states->size();j++) {
            cout<<getMarquageName(*lscc_states->at(j))<<" , ";
        }
        cout<<endl;
    }
}


void PetriNet::printMetaStateEx(MetaState *ms) {
    vector<Marking *>* list_states=ms->getListMarq();

    cout<<"Number of states: "<<list_states->size()<<endl;
    cout<<"Number of arcs: "<<ms->getArcCount()<<endl;
    //for (ms->
    /*for (int i=0;i<list_arcs->size();i++) {
        cout<<"Arc n°"<<i<<": ("<<getMarquageName(*list_arcs->at(i).getSource())<<" ===";
        cout<<list_arcs->at(i).getTransition()->getName()<<"==> ";
        cout<<getMarquageName(*list_arcs->at(i).getDestination())<<")"<<endl;
    }
    vector<SCC*>* list_sccs=ms->getListSCCs();
    cout<<"Number of SCCs: "<<list_sccs->size()<<endl;
    for (int i=0;i<list_sccs->size();i++) {
        cout<<"SCC n°"<<i<<endl;
        vector<Marking*>* lscc_states=list_sccs->at(i)->getListStates();
        for (int j=0;j<lscc_states->size();j++) {
            cout<<getMarquageName(*lscc_states->at(j))<<" , ";
        }
        cout<<endl;
    }*/
}


string PetriNet::getSCCName(SCC *scc) {
    return getMarquageName(*scc->getListStates()->at(0));
}

