// ConstructPetriFromFile.cpp: implementation of the CConstructPetriFromFile class.
//
//////////////////////////////////////////////////////////////////////


#include "ConstructPetriFromFile.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConstructPetriFromFile::CConstructPetriFromFile()
{
	fp=NULL;
}

CConstructPetriFromFile::~CConstructPetriFromFile()
{
	
	
}

void CConstructPetriFromFile::setFileName(const string name)
{
	m_nom_fichier=name;
}
string CConstructPetriFromFile::getFileName() {
    return m_nom_fichier;
}
ModularPetriNet* CConstructPetriFromFile::getModularPetrinet()
{
	ModularPetriNet* MPetri=NULL;

	
	fp=fopen(m_nom_fichier.c_str(),"r");
	if (fp) {
		//Cr�er le r�seau de Petri
		cout<<"Loading modular Petri net from file..."<<endl;
		MPetri=new ModularPetriNet;
		//Lecture de nombre de modules
		unsigned int nb_modules=0;
		string mot;
		getNextWord();
		mot=getNextWord();		
		nb_modules=atoi(mot.c_str());
		cout<<"This modular Petri net consists of "<<mot<<" modules"<<endl;
		cout<<"Displaying informations related to the modular Petri net"<<endl;
		//Initialisation
		PetriNet* petri;
		vector<Place> liste_places;
		vector<Transition> liste_transitions;
		for (unsigned int ii=0;ii<nb_modules;ii++){
			int code_transition=0;
			cout<<"Module n�"<<(ii+1)<<endl;


			getNextWord();	
			//
			// Lire les libell�s de places
			string chaine;
			petri=new PetriNet();
			// Sp�cifier le num du module
			petri->setNumero(ii);
			Place place;
			liste_places.clear();
			/******************************************/
			/** Ajout des places et de leurs marquages*/
			/******************************************/
			do {					
				int marquage=00;
				mot=getNextWord();			
				while (mot=="Marquage") {
					mot=getNextWord();
					marquage=atoi(mot.c_str());
//					printf("marquage :%d \n",marquage);
					
					do {			
						mot=getNextWord();
						if (mot!="Marquage" && mot!="End") {
							place.setName(mot.c_str());
							place.setTokens(marquage);
							liste_places.push_back(place);
							cout<<"Added place: "<<mot.c_str()<<endl;
						}
					} while (mot!="Marquage" && mot!="End");					
				}				
			} while (mot!="End");			
			petri->addListPlaces(liste_places);
			/****************************************/
			/** Lecture de libell�s de transitions **/
			/****************************************/
			getNextWord();
			Transition transition;
			liste_transitions.clear();
			do {
				mot=getNextWord();				
				if (mot!="End") {
					transition.setName(mot);
					transition.setCode(code_transition);
					code_transition++;
//					printf("Transition ajout�e :%s*\n",mot.c_str());
					liste_transitions.push_back(transition);			
				}
			} while (mot!="End");
			petri->addListTransitions(liste_transitions);

			/****************************************/
			/** Lecture des entr�es de transitions **/
			/****************************************/

			getNextWord();
			vector<string> liste_places_entrees;

			vector<int> liste_poids;
			string nom_transition;
			do {
				nom_transition=getNextWord();
				if (nom_transition!="End") {
					cout<<"Transition: "<<nom_transition.c_str()<<" has as place inputs ";
					string item;
					while (item!="]") {
						item=getNextWord();
						if (item=="-")
						{
							string poids;
							poids=getNextWord();
					        int poids_entier=atoi(poids.c_str());
							liste_poids.pop_back();
							liste_poids.push_back(poids_entier);
						}
						else
						if (item!="[" && item!="]") {
							cout<<item.c_str()<<"<";
							liste_places_entrees.push_back(item);
							liste_poids.push_back(1);
						}						
					}
					if (liste_places_entrees.size()==liste_poids.size())
						petri->addPlacesEntrees(nom_transition,liste_places_entrees,liste_poids);
					else cout<<"Erreur liee au poids...";
					liste_places_entrees.clear();
					liste_poids.clear();
				}
				cout<<endl;
			} while (nom_transition!="End");
			
			

			/****************************************/
			/** Lecture des sorties de transitions **/
			/****************************************/
			getNextWord();
			vector<string> liste_places_sorties;
			liste_poids.clear();
			do {	
				nom_transition=getNextWord();
				if (nom_transition!="End") {					
					cout<<"Transition: "<<nom_transition.c_str()<<" has as output places ";
					string item;
					while (item!="]") {
						item=getNextWord();
						if (item=="-")
						{
							string poids;
							poids=getNextWord();
					        int poids_entier=atoi(poids.c_str());
							liste_poids.pop_back();
							liste_poids.push_back(poids_entier);
						}
						else
						if (item!="[" && item!="]") {
							cout<<">"<<item.c_str();
							liste_places_sorties.push_back(item);
							liste_poids.push_back(1);
						}						
					}			
					if (liste_places_sorties.size()==liste_poids.size())
						petri->addPlacesSorties(nom_transition,liste_places_sorties,liste_poids);
					else cout<<"erreur liee au poids...";
					liste_places_sorties.clear();
					liste_poids.clear();
				}
				cout<<endl;
			} while (nom_transition!="End");
			MPetri->addModule(petri);			
		}
		/*************************************************/
		/** Lecture de transititions de syn,chronisation**/
		/*************************************************/
		cout<<"Determining synchronised transitions..."<<endl;
		getNextWord();
		string transition;
		vector<string> liste_sync;
		string temp;
		do {
			transition=getNextWord();
			if (transition!="End") {
				cout<<transition.c_str()<<" ";
				temp=transition;
				liste_sync.push_back(temp);
			}

		} while (transition!="End");
		MPetri->setSync(liste_sync);		
	}
	else {
		cout<<"Error: can't open file";
	}
	fclose(fp);
	return MPetri;
}



string CConstructPetriFromFile::getNextWord()
{
	string separateur=",;(:)";
	string chaine;
	char c='\0';
	while (c<=(char)32 || separateur.find_first_of(c)!=-1) {
		c=fgetc(fp);
	}	
	bool quit=false;
	while (c>32 && !quit) {
		if (separateur.find_first_of(c)!=-1) {
			quit=true;			
		}
		else {
			chaine+=c;
		    c=fgetc(fp);
		}
	}
	return chaine;
}

