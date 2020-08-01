////////////////////////
//       Imports
////////////////////////

#include <complex>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sys/time.h>
#include <stdio.h>
#include <bitset>
#include <stdlib.h>
#include <random>
#include <list>
#include <unordered_set>

////////////////////////////

#define L 20        // Genome Length
#define N 1048576   // 2^L, Number of possible species

using namespace std;

////////////////////////////



////////////////////////////
//      Parameters
///////////////////////////

int t_gens = 0;				    // t - current generation number 

double C = 100; 			    //scaling parameter for interspecies interactions
double sigma = 0;			    //scaling parameter for species environment interactions. NOT INCLUDED IN BASE MODEL

double pKill = 0.20;			    //probability of killing an individual
double pMute = 0.01;            //probability of a mutation occurring

double pSleep = 0.3;            
double pWake = 0.5;

double Amu = 0.1; 	            //environmental scaling factor - "resource abundance"
double Smu = 0.01; 	            //environmental scaling factor - "resource abundance"

const int INITIAL_POPULATION = 50;   //starting population
int Npop = INITIAL_POPULATION;  		//total population tracker
int Apop = INITIAL_POPULATION;        //total awake population tracker
int Spop = 0;					//total dormant population tracker

//const double DORM_BOUND = 0.3;

const int MAX_GENS = 5000;      //termination condition

const double theta = 0.25;      //density of non-zero interspecies interactions

bool Jran1[N];				    //helper list for implementing interspecies coupling matrix efficiently
double Jran2[N];			    //helper list for implementing interspecies coupling matrix efficiently
double Jran3[N];                //helper list for implementing interspecies coupling matrix efficiently




///////////////////////////
// Random Numbers:

mt19937 mt_generator;  //Mersenne twister engine

double mt_normalise = 1.0 / (double)( 1.0 + mt_generator.max());

inline double mt_rand(){
    return (double)mt_generator()*mt_normalise;
}

inline double mt_rand_sym(){
    return 2.0f*mt_generator()*mt_normalise - 1.0;
}

inline int random_species(){
    return (int)(N * mt_rand());
}
///////////////////////////




///////////////////////////
// Species Object:
// EXTENDED FROM BASE TO INCLUDE DORMANT POP

class Species {
public:
	int sID;			//species ID
	bitset<L> bin_sID;	//binary genome - sID in binary
	int population;
	int awake_population; //number of individuals non-dormant belonging this species
	int asleep_population; //number of individuals dormant belonging this species 
	
	//initialise
	Species(int sID_, int pop_) : sID(sID_), awake_population(pop_), population(pop_) {
		bitset<L> tmp (sID);
		bin_sID = tmp;
	}	
};

list<Species> ecology;  //List of extant species
unordered_set<int> encountered; //List of all encountered species

	

inline void addPop(list<Species>::iterator s){
	int t = Apop - s->awake_population;
	s->awake_population++;
	s->population++;

	Npop++;
	Apop++;

	if (t != Apop - s->awake_population){
		cout << "Error in AddPop";
	}
} 

inline void sleepPop(list<Species>::iterator s){
	s->awake_population--;
	s->asleep_population++;

	Spop++;
	Apop--;
} 

inline void wakePop(list<Species>::iterator s){
	s->awake_population++;
	s->asleep_population--;

	Spop--;
	Apop++;
} 

inline void setPop(list<Species>::iterator s){
	s->awake_population = 1;
	s->population = 1;
	s->asleep_population = 0;

	Npop++;
	Apop++;
} 

inline void removePop(list<Species>::iterator s){
	int t = Apop - s->awake_population;
	s->awake_population--;
	s->population--;

	Npop--;
	Apop--;

	if (t != Apop - s->awake_population){
		cout << "Error in RemovePop";
	}
} 

list<Species>::iterator searchNode(list<Species> &ecology, int n) {
	for (list<Species>::iterator cur=ecology.begin(); cur != ecology.end(); ++cur){
		if(cur->sID == n) return cur;
	}
	return ecology.end();
}

int findTrueAwakePopulation(list<Species> &ecology){
	int out = 0;
	for (list<Species>::iterator cur=ecology.begin(); cur != ecology.end(); ++cur){
		out += cur->awake_population;
	}
	return out;
}

int findTrueAsleepPopulation(list<Species> &ecology){
	int out = 0;
	for (list<Species>::iterator cur=ecology.begin(); cur != ecology.end(); ++cur){
		out += cur->asleep_population;
	}
	return out;
}
//////////////////////////





// Initialises everything to do with the model
// Beginning with setting the random number
// generators to comply with the seed given.
//
//
//
//
inline void initialise_model(int model_seed){

    default_random_engine generator(model_seed + 123);
    mt_generator = mt19937(model_seed);

    double oC = sqrt(C);
	double omu = sqrt(Amu);
    
    //Generate the interaction matrix
	normal_distribution<double> distribution(0.0, 1.0);
    for(int i=0; i<N; i++){
		( mt_rand() < theta ) ? Jran1[i] = true : Jran1[i] = false;
		Jran2[i] = oC*distribution(generator);
		Jran3[i] = oC*distribution(generator);
    }

    // Create the initial population with a random species
    int rs = random_species();
	encountered.insert(rs);
	ecology.emplace_front(rs, INITIAL_POPULATION);
}



inline list<Species>::iterator kill_awake(){
	double rand = mt_rand();
	double sum = 0;
	for (list<Species>::iterator cur=ecology.begin(); cur != ecology.end(); ++cur){
		sum += cur->awake_population;  //find total population of currently iterated species
		if( Apop*rand <= sum ){  //gives each "individual" an equal chance to be chosen
			if( mt_rand() < pKill ){	//INIDIVIDUAL CHOSEN
				removePop(cur);

				if(cur->population == 0){  //if species is now extinct, remove from list
					ecology.erase(cur);
				}			

				return ecology.end();	//otherwise return ecology.end() (have to return a species object)

			} else {
				return cur; //don't kill an individual
			}
		}
	}

    // ERROR HANDLING (NO INDIVIDUAL CHOSEN)
	
	cerr << "kill failed! "<< endl;
	cerr << "Apop = " <<  Apop << endl;
	cerr << "AActual = " << findTrueAwakePopulation(ecology) << endl;
	cerr << "rand " << rand << endl;
	for (list<Species>::iterator cur=ecology.begin(); cur != ecology.end(); ++cur){
		if (cur->awake_population > 0) cerr << " -Species pop: " << cur->awake_population << endl;
	}
	exit(1);
}



//choose a random individual
inline list<Species>::iterator choose_awake(){
	double rand = mt_rand();
	int sum = 0;
	for (list<Species>::iterator cur=ecology.begin(); cur != ecology.end(); ++cur){
		sum += cur->awake_population;
		if( Apop*rand <= sum ){
			return cur;
		}
	}
	
    // ERROR HANDLING (NO INDIVIDUAL CHOSEN)
	cerr << "awake choose failed! " << endl;
	cerr << "Apop = " <<  Apop << endl;
	cerr << "AActual = " << findTrueAwakePopulation(ecology) << endl;
	cerr << "rand = " << rand << endl;
	cerr << "rand * Apop = " << rand * Apop << endl;
	exit(1);
}

inline list<Species>::iterator choose_asleep(){
	double rand = mt_rand();
	int sum = 0;
	for (list<Species>::iterator cur=ecology.begin(); cur != ecology.end(); ++cur){
		sum += cur->asleep_population;
		if( Spop*rand <= sum ){
			return cur;
		}
	}
	
    // ERROR HANDLING (NO INDIVIDUAL CHOSEN)
	cerr << "asleep choose failed! " << endl;
	cerr << "Spop = " <<  Spop << endl;
	cerr << "SActual = " << findTrueAsleepPopulation(ecology) << endl;
	cerr << "rand = " << rand << endl;
	cerr << "rand * Spop = " << rand * Spop << endl;
	exit(1);
}





/////////////////////////////////////////////////////////////
//  Finding pOff                                             

inline double calc_HI(list<Species>::iterator elem){
	double sum = 0; 
	for (list<Species>::iterator cur=ecology.begin(); cur != ecology.end(); ++cur){ //loop over each species in ecology
		if(cur->bin_sID != elem->bin_sID){	                                        //no self interaction
			bitset<L> bin_inter = cur->bin_sID ^ elem->bin_sID;		                //trick to implement coupling matrix
			long int s_inter = bin_inter.to_ulong();				
			if( Jran1[s_inter] ){
				sum += Jran2[s_inter] * Jran3[cur->sID] * cur->awake_population;	//inter species
			}
		}
	}
	return sum;
}

inline double calc_H(list<Species>::iterator elem){
	return calc_HI(elem)/Apop - Amu*Apop; //- Smu*Spop;
}

//reproduction probability	
inline double poff(list<Species>::iterator elem){
    return 1.0/(1.0 + exp(-calc_H(elem)));
}

/////////////////////////////////////////////////////////////






inline bitset<L> mutateOffspring(list<Species>::iterator elem){
    bitset<L> bin_new;
    for(int i=0; i<L; i++){
		if(mt_rand() < pMute){ bin_new[i] = !elem->bin_sID[i]; }	//each bit can mutate with probability pmut
		else{ bin_new[i] = elem->bin_sID[i]; }					
	}
    return bin_new;
}   


//generate offspring of species 'elem' with mutation
inline void asexual(list<Species>::iterator elem){
	int del = findTrueAwakePopulation(ecology) - Apop;
	removePop(elem);
	

	bitset<L> bin_newA = mutateOffspring(elem);	//new individual genomes
    bitset<L> bin_newB = mutateOffspring(elem);

	if( bin_newA != elem->bin_sID ){ //if new species
		list<Species>::iterator tmpNode = searchNode(ecology, bin_newA.to_ulong() ); //have we seen this species already?
		if(tmpNode == ecology.end()){ //if new species not on list
			ecology.emplace_front(bin_newA.to_ulong(), 1); //add to lsit
			encountered.insert(bin_newA.to_ulong());
			tmpNode = searchNode(ecology, bin_newA.to_ulong());
			setPop(tmpNode);
		} else { //if new species already on list
			addPop(tmpNode);
		}	
	} else {  //no mutation
		addPop(elem); //increase original species count by 1
	}

    if( bin_newB != elem->bin_sID ){ //if new species
		list<Species>::iterator tmpNode = searchNode(ecology, bin_newB.to_ulong() ); //have we seen this species already?
		if(tmpNode == ecology.end()){ //if new species not on list
			ecology.emplace_front(bin_newB.to_ulong(), 1); //add to lsit
			encountered.insert(bin_newB.to_ulong());
            tmpNode = searchNode(ecology, bin_newB.to_ulong());
			setPop(tmpNode);
		} else { //if new species already on list
			addPop(tmpNode); //increase that species count by 1
		}	
	} else {  //no mutation	
		addPop(elem); //increase original species count by 1
	}

	if (del != findTrueAwakePopulation(ecology) - Apop) cerr << "ERROR IN ASEXUAKL";
}

inline void fall_Dormant(list<Species>::iterator elem){
	if (elem->awake_population > 0){
		sleepPop(elem);
	}
}

inline void wake_Up(list<Species>::iterator elem){
	if (elem->asleep_population > 0){
		wakePop(elem);
	}	
}


inline void print_file(ofstream &pop_file, double percent=0.05){ //percent = condition for being in the core
	
	int max = 0; 
	int diversity = 0;
	for (list<Species>::iterator cur=ecology.begin(); cur != ecology.end(); ++cur){
		if( cur->awake_population >  max ){ max = cur->awake_population; }
		++diversity;
	}

	int tAPop = 0;
	int core_pop = 0;
	int core_size = 0;
	for (list<Species>::iterator cur=ecology.begin(); cur != ecology.end(); ++cur){
		tAPop += cur->awake_population;
		if( cur->awake_population > percent * (double) max ){ 
			core_pop += cur->awake_population;
			++core_size;
		}
	}

	
	//generation number   number of individuals, awake, asleep 		   number of extant species,  number of species seen, individuals in core,   species in core
	pop_file << t_gens << "," << Npop << "," << Apop << "," << tAPop << "," << Spop << "," << diversity << "," << encountered.size() << "," << core_pop << "," << core_size << endl;			

}









//////////////////////////
//        Main
//////////////////////////

int main(int argc, char *argv[]){
    int it;
    int seed;
    string path;
	bool verbose = false;

    if(argc < 2)
    {
        cerr << "No Seed or Path Inputted, Assuming Default (0, .\\)" << endl;

        it = 0;
        seed = 12345;
        path = "";
    }
    else
    {
        int it = atoi(argv[1]);
        seed = 123*it + 12345;
        path = argv[2]; 
    }

    initialise_model(seed);
    int t = 0;                     //intra-generational counter
    double lgen = Apop / pKill;    //length of current generation

    string filename =  "dormplot_seed" + to_string(it);
    filename += "_C"+ to_string(C); 
	filename += "_mu"+ to_string(Amu); 
	filename += "_theta" + to_string(theta); 
	filename += "_pKill" + to_string(pKill); 
	filename += "_pMute" + to_string(pMute); 
	filename += "_pSleep" + to_string(pSleep);
	filename += "_pWake" + to_string(pWake);
    filename += "_pInit" + to_string(INITIAL_POPULATION);
 	filename += "_L" + to_string(L);
	filename +=".csv";

    filename = path + filename;
    ofstream pop_file; pop_file.open (filename.c_str());

    pop_file << "generation,Npop,Apop,ActualApop,Spop,diversity,encountered,core_pop,core_size" << endl;

    do{
		

		list<Species>::iterator sID = kill_awake();       //choose an individual and kill with prob pkill
        
		if(Npop == 0){ break; }                     //if population totally killed off, end
		
		if(sID == ecology.end()) sID = choose_awake();	//if we killed the individual, choose another one
		
		double pOff = poff(sID);

		if(pSleep > pOff){ // Species has an inate understanding of environment and if it's particularly bad it'll fall dormant
			fall_Dormant(sID);
			if (verbose){
				cout << "	-Fell Dormant: " << pOff << endl;
			}
		}
		


		if( mt_rand() < pOff ){		//individual reproduces with probability poff
			asexual(sID);		                    //reproduce asexually
		}

		if (sID->asleep_population == 0) sID = choose_asleep();
		if(pWake < pOff){
			wake_Up(sID);
			if (verbose){
				cout << "	-Awoke: " << pOff << endl;
			}
		}

		if (verbose){ // for debugging purposes
			cout << endl << "SUBGENERATION " << t << ":" << endl;
			cout << "   NPop: " << Npop << endl;
			cout << "   APop: " << Apop << endl;
			cout << "   Actual: " << findTrueAwakePopulation(ecology) << endl;
			for (list<Species>::iterator cur=ecology.begin(); cur != ecology.end(); ++cur){
				cout << "   Species: " << cur->bin_sID << " (" << cur->bin_sID.to_ulong() << ")" << endl;
				cout << "         APop: " << cur->awake_population << endl;
				cout << "         Pop:  " << cur->population << endl;
			}
			cin;
		}

		++t; //counter

		
		if(t >= lgen){	//generation is over
			t = 0; ++t_gens; lgen = Apop / pKill; //recalculate generation length
			print_file(pop_file);

			double progress = float(t_gens) / float(MAX_GENS);
			int barWidth = 70;

    		cout << "[";
    		int pos = barWidth * progress;
   			for (int i = 0; i < barWidth; ++i) {
        		if (i < pos) cout << "=";
        		else if (i == pos) cout << ">";
        		else cout << " ";
    		}
    		cout << "] " << int(progress * 100.0) << " %\r";
    		cout.flush();		
		}
		
	}while(t_gens < MAX_GENS); //stopping condition. run for MAX_GENS
	pop_file.close();  //Closes the file stream.
	//core_file.close();

	return 0;	

}