#include "Environnement.h"
#include "Individu.h"
#include "Simulation.h"

#include <sstream>
#include <fstream>


#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>    // std::random_shuffle
#include <cstdlib>    //time
//Constructeurs

Simulation::Simulation (){
	//Déclaré privé pour empêcher son usage.
	
	//~ MAP = new Environnement();
	//~ pop = nullptr;
	//~ T = 0;
	//~ Ainit = 0;
	//~ D = 0;
	//~ poursuivre = 0;
	//~ Pmut = 0;
	//~ Pdeath = 0;
		
	//~ Raa = 0;
	//~ Rbb = 0;
	//~ Rab = 0;
	//~ Rbc = 0;
	
	//~ Wmin = 0;
	//~ Tfini = 0;
	
}

//:param nT: nombre de tours avant changement de milieu : 
//durée de la simulation
Simulation::Simulation (unsigned int nW, unsigned int nH, 
						 unsigned int nT, double nAinit){
							
	T = nT; 
	Ainit = nAinit;
	D = 0.1;
	W = nW;
	H = nH;
	
	this->MAJparametres ();
	
	//Environnement :
	MAP = new Environnement(W, H, Ainit);
	
	//Tableau de population, rempli de B, puis de 50% de A.
	pop = new Individu**[W];
	
	int nbA = 0;
	int max = (W*H)/2;
	
	for (unsigned int i = 0 ; i<W ; i++){
		pop[i] = new Individu*[H];
		for (unsigned int j = 0 ; j<H ; j++){
			pop[i][j] = new Individu(i,j, 1);
		}
	}
	unsigned int i = 0;
	unsigned int j = 0;
	while (nbA <= max){
			i = ((double) std::rand()/(double) RAND_MAX)*((double) (W));
			j = ((double) std::rand()/(double) RAND_MAX)*((double) (H));
			if ((pop[i][j])->getgen ()){
				pop[i][j]->setgen(0);
				nbA++;
			}
	}
	situation = new int[3];
	situation[0] = nbA;
	situation[1] = W*H - nbA;
	situation[2] = 0; //cohabitation
	
}


Simulation::Simulation (unsigned int nW, unsigned int nH, 
						 unsigned int nT, double nAinit, double nD){
	T = nT;
	Ainit = nAinit;
	D = nD;
	W = nW;
	H = nH;
	
	this->MAJparametres ();
	
	//Environnement :
	MAP = new Environnement(W, H, Ainit, D);
	
	//Tableau de population, rempli de B, puis de 50% de A.
	pop = new Individu**[W];
	
	int nbA = 0;
	int max = (W*H)/2;
	
	for (unsigned int i = 0 ; i<W ; i++){
		pop[i] = new Individu*[H];
		for (unsigned int j = 0 ; j<H ; j++){
			pop[i][j] = new Individu(i,j, 1);
		}
	}
	unsigned int i = 0;
	unsigned int j = 0;
	while (nbA <= max){
			i = ((double) std::rand()/(double) RAND_MAX)*((double) (W));
			j = ((double) std::rand()/(double) RAND_MAX)*((double) (H));
			if ((pop[i][j])->getgen ()){
				pop[i][j]->setgen(0);
				nbA++;
			}
	}

	situation = new int[3];
	situation[0] = nbA;
	situation[1] = W*H - nbA;
	situation[2] = 0; //cohabitation
}

//Destructeurs

Simulation::~Simulation () {
	for (unsigned int i = 0 ; i<W ; i++){
		for (unsigned int j = 0 ; j<H ; j++){
			delete pop[i][j];
		}
		delete[] pop[i];
	}
	delete[] pop;
	delete MAP;
	delete situation;
}


//Méthodes de simulation


int Simulation::Simulate (){
	using namespace std;
	int temps = 0;
	this->Metabolisme ();
	
	int PasGraph = 1;
	int graph = 0;
	stringstream s;
	s << "Etat" << ".ppm";	
	string title = s.str();
	
	//TODO : lecture du fichier texte pour paramètres
	while (poursuivre){

			//Les métabolites externes diffusent dans l'environnement
		MAP->Diffusion ();
			//Certains individus meurent, leurs métabolites sont rejetés
			//et ils passent de vivant à mort
		this->Hecatombe ();
			//Compétition pour la reproduction. parcours aléatoire des 
			//Cases où les individus sont mort (cad vides). et compétition 
			//entre les individus extérieurs. L'individu choisi voit ses C
			//en métabolite divisées par deux (donc sa fitness aussi) et
			//A l'espace vide est attribué par opérateur par copie (après 
			//DESTRUCTION du précédent) cet individu.
			//Après division, un individu ne peut se reproduire dans le même tour.
		this->GuerreSexuelle ();
			//Mutation aléatoire des organisme independemment les uns des autres.
		this->Mutation ();	//OK : les divisées d'avant mutent et leur fitness est actualisée
			//Métabolisme des individus. La FITNESS est actualisée pour tous.
		this->Metabolisme (); //actualiser la fitness egalement, remet divide à true
			//Affichage de la boîte de pétri
		
		//enregistrement de la boîte de Pétrie 
		//~if (temps%PasGraph == 0){
			//~//this->Afficher ();
			//~s.str("");
			//~s <<"Etat"<< graph << ".ppm"; //~/Documents/ProjetCEvolutionRep/
			//~saveGraph(s.str());
			//~MAP->AfficherB ();
			//~graph++;
		//~}
		
		//Arrêt conditionnel
		temps++;
		this->JeContinue (temps);
		if (temps%T == 0){
			//this->Afficher ();
			//LECTURE DES PARAMèTRES.
			MAP->clean ();
		}
			
	}
}

void Simulation::Hecatombe (){
	
	bool tokill = true;
	Individu* p = nullptr;
	
	for (unsigned int i = 0 ; i<W ; i++){
		for (unsigned int j = 0 ; j<H ; j++){
			p = pop[i][j];
			if (p->isalive()){
				tokill = (((double) std::rand()/RAND_MAX)<=Pdeath);
				if(tokill){
					p->kill();
					p->stopdivide ();
					situation[p->getgen()]--;
					
					MAP->setA(i,j,MAP->getA(i,j) + p->getA());
					MAP->setB(i,j,MAP->getB(i,j) + p->getB());
					MAP->setC(i,j,MAP->getC(i,j) + p->getC());
				}
			}
		}
	}
}
	
//programme :
//trouver les gap : c mortes et divide = true (ie : pas morte pendant ce tour)
	//Parcourir les cases voisines et sélectionner le plus fort à même de se battre
	//Diviser par deux ses concentrations, donc sa fitness, et passer à 0 
	//le booleen candivide puis
	//remplir le gap par le constructeur par copie.
void Simulation::GuerreSexuelle (){
	//Parcours et enregistrement de toutes les cases vides dans un tableau 2D
	Individu* p = nullptr;
	Individu* pfille = nullptr;
	std::vector <int*> gap = {};
	int* tab = nullptr;

	
	for (int i = 0; i<W ; i++){
		for(int j = 0 ; j<H ; j++){
			p = pop[i][j];
			if((!(p->isalive())) and (p->candivide ())){
				tab = new int[2];
				tab[0] = i;
				tab[1] = j;
				gap.push_back(tab);

			}
		}
	}
	int n = gap.size()	;
	//Mélange des éléments - (controle dimension n fait)
	if(n>2){
		std::random_shuffle(&gap[0], &gap[n-1]);
	}
	
	//initialisation des variables intermédiaires
	int ni = 0;
	int nj = 0;
	int nf = 0;
	
	double Rocf;
	std::vector <int*> vois (0);
	int* pos = nullptr; 
	
	//pour chaque gap :

	for (int k = 0 ; k<n ; k++){
		Rocf = -1;
		for (int di = -1 ; di<=1; di++){
			for(int dj = -1 ; dj<=1 ; dj++){
				if((di !=0) || (dj != 0)){
					//parcours des cases voisines
					ni = gap[k][0] + di;
					nj = gap[k][1] + dj;
					if (ni<0){ni = W-1;}
					if (ni == W){ni = 0;}
					if (nj<0){nj = H-1;}
					if (nj == H){nj = 0;}
					
					//pour chacune, si vivante et apte à se reproduire : 
					p = pop[ni][nj];
					nf = p->getfitness ();
					if (p->isalive () && p->candivide ()){
						if(nf == Rocf){
							pos = new int[2];
							pos[0] = ni;
							pos[1] = nj;
							vois.push_back(pos);
						}else if(nf > Rocf){
							if(vois.size() != 0){
								if(vois[0] != nullptr){
									for (std::vector< int* >::iterator it = vois.begin() ; it != vois.end(); ++it){	
										delete[] *it;
										*it = nullptr;
									}
								}
							}
							vois.clear();
							pos = new int[2];
							pos[0] = ni;
							pos[1] = nj;
							vois.push_back(pos);
							Rocf = nf;
						}
					}
				}
			}
		}
		if( Rocf > Wmin){
			//std::cout << "Reproduction avec fitness = "<< Rocf << " en case "<< Roci << "  " << Rocj<< std::endl ;
			//Choix du Rocco :
			nf = std::rand()%vois.size();
			ni = vois[nf][0];
			nj = vois[nf][1];
			
			p = pop[ni][nj]; //tu gagne Rocco !
			
			pfille = pop[gap[k][0]][gap[k][1]];
			pfille->DB2andCopy(p); //alors pfille ressemble exactement à p et divide passé à false
			
			
		
			situation[p->getgen ()]++;
		} //else do nothing
		
		if(vois.size() != 0){
			if(vois[0] != nullptr){
				for (std::vector< int* >::iterator it = vois.begin() ; it != vois.end(); ++it){	
					delete[] *it;
					*it = nullptr;
				}
			}
		} 
		vois.clear();
		//
	}

	 for (std::vector< int* >::iterator it = gap.begin() ; it != gap.end(); ++it){
		delete[] *it;
		*it = nullptr;
	} 
	gap.clear();
}

void Simulation::Metabolisme (){
	Individu* p = nullptr;
	double dt = 0.1;
	
	double Aout = 0;
	double A = 0;
	double Bout = 0;
	double B = 0;
	double C = 0;
	
	for (unsigned int i = 0 ; i<W ; i++){
		for (unsigned int j = 0 ; j<H ; j++){
			
			p = pop[i][j];
			
			if (p->isalive ()){
				
				if (p->getgen ()){
					
					//Métabolisme B
					for (int t = 0; t<10 ; t++){
						Bout = MAP->getB(i,j);
						B = (p->getphen())[1];
						C = (p->getphen())[2];
						//Bout
						MAP->setB(i,j,Bout - dt*Bout*Rbb);
						//B
						p->setB(B + dt*(Bout*Rbb - B*Rbc));
						//C
						p->setC(C + dt*B*Rbc);	
					}
					
				}else{
					
					//Métabolisme A
					for (int t = 0; t<10 ; t++){					
						Aout = MAP->getA(i,j);
						A = (p->getphen())[0];
						B = (p->getphen())[1];
						
						//Aout
						MAP->setA(i, j, Aout - dt*Aout*Raa);
						//A
						p->setA(A + dt*(Aout*Raa - A*Rab));
						//B
						p->setB(B + dt*A*Rab);
					}
					
				} //end else
				
			} //end isalive
			
		} //end for
	} //end for
	
	
	//Actualisation de la fitness
	this->MAJfitness ();

	//remise à true de divide
	for (unsigned int i = 0 ; i<W ; i++){
		for (unsigned int j = 0 ; j<H ; j++){	
			p = pop[i][j];
			p->godivide();

		}
	}
	
}

void Simulation::Mutation (){
	
	bool mutate = true;
	Individu* p = nullptr;
	
	for (unsigned int i = 0 ; i<W ; i++){
		for (unsigned int j = 0 ; j<H ; j++){
			
			p = pop[i][j];
			if (p->isalive() && (!p->candivide ())){
				mutate = (((double) std::rand()/RAND_MAX)<=Pmut);
				if (mutate){
					situation[p->getgen()]--;
					p->mutate();
					situation[p->getgen()]++;
				}
			}
			
		}
	}
	
}

void Simulation::MAJfitness (){
	Individu* p = nullptr;
	
	for (unsigned int i = 0 ; i<W ; i++){
		for (unsigned int j = 0 ; j<H ; j++){
			
			p = pop[i][j];
			if (p->isalive ()){
				this->MAJfitnessij(i, j);
			}
			
		}
	}
}

void Simulation::MAJfitnessij (int i, int j){
	Individu* p = nullptr;
	p = pop[i][j];
	p->setfitness((p->getphen ())[p->getgen ()]);
	if(p->getfitness () < Wmin){
		p->setfitness(0);
		p->stopdivide();
	}	
}

//Méthodes auxiliaires

void Simulation::Afficher (){
	
	Individu* p = nullptr;
	this->Situation ();
	
	std::cout << "La simulation est dans l'état : " << situation[2] << " avec : "<< std::endl ;
	std::cout << situation[0]<< " individus de phétonype A et "<< situation[1] << " individus de phétonype B." <<std::endl;
	
	for (unsigned int j = 0 ; j<H ; j++){
		for (unsigned int i = 0 ; i<W ; i++){
			p = pop[i][j];
			if (p->isalive()){
				std::cout << p->getgen () << " ";
			}else{
				std::cout << "X" << " ";
			}
		}
		std::cout << "" << std::endl;
	}
	std::cout << "L'état des ressources en A est le suivant : "<< std::endl ;
	//MAP->AfficherA (); //TODO : ENLEVER APRES TESTS
	
}

void Simulation::saveGraph (std::string nameout){
	
	using namespace std;
	cout << "On save dans "<< nameout << endl;
	
	Individu* p = nullptr;
	
	std::ofstream f(nameout, ios::out | ios::trunc | ios::binary);
	
	
	f << "P6\n" << 4*W << " " << H << "\n" << 255 << "\n";
	char a = 100;
	char b = 100;
	char c = 0;	
	double ratio = 255/Ainit;
	for (unsigned int i = 0 ; i<W ; i++){
			
		//Bactéries
		for (unsigned int j = 0 ; j<H ; j++){
			
			p = pop[i][j];
			
			
			if(!p->isalive ()){
				a = 0;
				b = 0;				 
			}else if (!p->candivide ()){
				a = 255;
				b = 255;				
			}else if (p->getgen ()){
				a = 200;
				b = 70;				
			}else{
				a = 70;
				b = 200;				
			}
				cout << "On ecrit" << i << " " << j << endl;
			  f.write(&a , sizeof(char));
			  f.write(&a , sizeof(char));
			  f.write(&b , sizeof(char));
				
		}
		
		// Show the map of Aout;
		for(int j = 0; j < H ; j++){
		  a = ((char) (MAP-> getA(i,j) * ratio) )%255;
		  f.write(&a , sizeof(char));
		  f.write(&c , sizeof(char));
		  f.write(&c , sizeof(char));
		}
		
		// Show the map of Bout;
		for(int j = 0; j < H ; j++){
		  a = ((char) (MAP-> getB(i,j) * ratio) )%255;
		  f.write(&c , sizeof(char));
		  f.write(&a , sizeof(char));
		  f.write(&c , sizeof(char));
		}
		
		// Show the map of Cout;
		for(int j = 0; j < H ; j++){
		  a = ((char) (MAP-> getC(i,j) * ratio) )%255;
		  f.write(&a , sizeof(char));
		  f.write(&a , sizeof(char));
		  f.write(&a , sizeof(char));
		}		
		
	}
	
	//f.close();
}


//modifie situation par code pour la situation sur le plateau:
// -1 : extinction ;
// 0 : cohabitation ;
// 1 : S (gen B) éteinte.


int Simulation::Situation (){
	if (situation[1] == 0){
		if (situation[0] == 0){
			situation[2] = -1; //extinction
		}else{
			situation[2] = 1; //S éteinte (plus de gen B)	
		}
	}else{
		situation[2] = 0; //cohabitation. Cas genA éteint non prévu.
	}
	return situation[2];
}

//Mise en place de conditions d'arrêt du système.
//TODO : changer nmax
void Simulation::JeContinue (int temps){
	if (temps > Tfini){
		std::cout << T <<  " " << Ainit << " " <<  this->Situation ()<< std::endl;

		poursuivre = false;
	}
	if (situation[2] == -1){
		poursuivre = false;
	}
}

void Simulation::MAJparametres (){
	//TODO : import des paramètres depuis fichier texte
	Pmut = 0;
	Pdeath = 0.02;
		
	Raa = 0.1;
	Rbb = 0.1;
	Rab = 0.1;
	Rbc = 0.1;	
		
	Wmin = 0.001;
	Tfini = 10000;
	poursuivre = true;
}























