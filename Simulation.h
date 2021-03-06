
#ifndef SIMULATION_H_
#define SIMULATION_H_
#include <iostream>
#include <list>

class Simulation {
	
	public:
	
		//constructeurs
		Simulation (unsigned int nW, unsigned int nH, unsigned int nT, double nAinit);
		Simulation (unsigned int nW, unsigned int nH,  unsigned int nT, double nAinit, double nD);
	
		//destructeur
		~Simulation ();
		
		//méthodes de la simulation
		int Simulate();
		
		void Hecatombe ();
		void GuerreSexuelle ();
		void Mutation ();
		void Metabolisme ();
		void MAJfitness ();
		void MAJfitnessij (int i, int j);
		
		//méthodes auxiliaires
		void JeContinue (int nbCycle);
		int Situation ();
		void Afficher ();	
		void saveGraph (std::string nameout);
		void MAJparametres ();			
		
	protected:
	
		Environnement* MAP;
		Individu*** pop;
		int* situation; //nbA, nbB, sit = 0 si cohab, -1 si extinction, 1 si seulement A.
			
		//variables		
		unsigned int T; //nb de tours d'un cycle
		double Ainit;
		double D;
		unsigned int W;
		unsigned int H;
		
		//Paramètres du modèle a pouvoir modifier en cours de route
		//begin
		bool poursuivre;
		double Pmut;
		double Pdeath;		
		double Raa;
		double Rbb;
		double Rab;
		double Rbc;
		double Wmin;
		double Tfini; //nb de tours total
		//end

	private:
	
		//constructeur par défaut non utilisable
		Simulation ();		
			
};

#endif //SIMULATION_H_
