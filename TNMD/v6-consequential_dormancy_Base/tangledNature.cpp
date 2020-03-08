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

double pKill = 0.2;			    //probability of killing an individual
double pMute = 0.01;            //probability of a mutation occurring
double pRevive = 0;             //probability of population 0 species reviving. NOT INCLUDED IN BASE MODEL

double mu = 0.1; 	            //environmental scaling factor - "resource abundance"

const int Npop_INITIAL = 500;   //starting population
int Npop = Npop_INITIAL;        //total population tracker

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
    return 2.0*mt_generator()*mt_normalise - 1.0;
}

inline int random_species(){
    return (int)(N * mt_rand());
}
///////////////////////////




///////////////////////////
// Species Object:

class Species {
public:
	int sID;			//species ID
	bitset<L> bin_sID;	//binary genome - sID in binary
	int population;		//number of individuals belonging this species
	
	//initialise
	Species(int sID_, int pop_) : sID(sID_), population(pop_) {
		bitset<L> tmp (sID);
		bin_sID = tmp;
	}	
};

list<Species> ecology;  //List of extant species
unordered_set<int> encountered; //List of all encountered species

list<Species>::iterator searchNode(list<Species> &ecology, int n) {
	for (list<Species>::iterator cur=ecology.begin(); cur != ecology.end(); ++cur){
		if(cur->sID == n) return cur;
	}
	return ecology.end();
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

    double oC = C;
	double omu = sqrt(mu);
    
    //Generate the interaction matrix
	normal_distribution<double> distribution(0.0, 1.0);
    for(int i=0; i<N; i++){
		( mt_rand() < theta ) ? Jran1[i] = true : Jran1[i] = false;
		Jran2[i] = distribution(generator);
		Jran3[i] = oC*distribution(generator);
    }

    // Create the initial population with a random species
    int rs = random_species();
	encountered.insert(rs);
	ecology.emplace_front(rs, Npop_INITIAL);
}



inline list<Species>::iterator kill(){
	double rand = mt_rand();
	double sum = 0;
	for (list<Species>::iterator cur=ecology.begin(); cur != ecology.end(); ++cur){
		sum += cur->population;  //find total population of currently iterated species
		if( Npop*rand <= sum ){  //gives each "individual" an equal chance to be chosen
			if( mt_rand() < pKill ){	//INIDIVIDUAL CHOSEN
				--cur->population;		//reduce species population by 1
                --Npop;	 //update total population

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
	cerr << "kill failed! Npop = " <<  Npop << endl;
	cerr << "rand " << rand << endl;
	exit(1);
}



//choose a random individual
inline list<Species>::iterator choose(){
	double rand = mt_rand();
	int sum = 0;
	for (list<Species>::iterator cur=ecology.begin(); cur != ecology.end(); ++cur){
		sum += cur->population;
		if( Npop*rand <= sum ){
			return cur;
		}
	}
	
    // ERROR HANDLING (NO INDIVIDUAL CHOSEN)
	cerr << "choose failed! Npop = " <<  Npop << endl;
	cerr << "rand " << rand << endl;
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
				sum += Jran2[s_inter] * Jran3[cur->sID] * cur->population;	//inter species
			}
		}
	}
	return sum;
}

inline double calc_H(list<Species>::iterator elem){
	return calc_HI(elem)/Npop - mu*Npop;
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
	++Npop; 	        //1 new individual (overall)
    --elem->population; //kill individual spliting (microbe asexual reproduction)

	bitset<L> bin_newA = mutateOffspring(elem);	//new individual genomes
    bitset<L> bin_newB = mutateOffspring(elem);

	if( bin_newA != elem->bin_sID ){ //if new species
		list<Species>::iterator tmpNode = searchNode(ecology, bin_newA.to_ulong() ); //have we seen this species already?
		if(tmpNode == ecology.end()){ //if new species not on list
			ecology.emplace_front(bin_newA.to_ulong(), 1); //add to lsit
			encountered.insert(bin_newA.to_ulong());
		} else { //if new species already on list
			++tmpNode->population; //increase that species count by 1
		}	
	} else {  //no mutation
		++elem->population; //increase species count by 1
	}

    if( bin_newB != elem->bin_sID ){ //if new species
		list<Species>::iterator tmpNode = searchNode(ecology, bin_newB.to_ulong() ); //have we seen this species already?
		if(tmpNode == ecology.end()){ //if new species not on list
			ecology.emplace_front(bin_newB.to_ulong(), 1); //add to lsit
			encountered.insert(bin_newB.to_ulong());
            
		} else { //if new species already on list
			++tmpNode->population; //increase that species count by 1
		}	
	} else {  //no mutation
		++elem->population; //increase species count by 1
	}
}




inline void print_file(ofstream &pop_file, double percent=0.05){ //percent = condition for being in the core
	
	int max = 0; 
	int diversity = 0;
	for (list<Species>::iterator cur=ecology.begin(); cur != ecology.end(); ++cur){
		if( cur->population >  max ){ max = cur->population; }
		++diversity;
	}

	int core_pop = 0;
	int core_size = 0;
	for (list<Species>::iterator cur=ecology.begin(); cur != ecology.end(); ++cur){
		if( cur->population > percent * (double) max ){ 
			core_pop += cur->population;
			++core_size;
		}
	}
	//generation number   number of individuals    number of species    individuals in core     species in core
	pop_file << t_gens << "," << Npop << "," << diversity << "," << encountered.size() << "," << core_pop << "," << core_size << endl;			

}









//////////////////////////
//        Main
//////////////////////////

int main(int argc, char *argv[]){
    int it;
    int seed;
    string path;

    if(argc < 2)
    {
        cerr << "No Seed or Path Inputted, Assuming Default (0, .\\)" << endl;

        it = 0;
        seed = 12345;
        path = ".\\";
    }
    else
    {
        int it = atoi(argv[1]);
        seed = 123*it + 12345;
        path = argv[2]; 
    }

    initialise_model(seed);
    int t = 0;                     //intra-generational counter
    double lgen = Npop / pKill;    //length of current generation

    string filename =  "popplot_seed" + to_string(it);
    filename += "_C"+ to_string(C); 
	filename += "_mu"+ to_string(mu); 
	filename += "_theta" + to_string(theta); 
	filename += "_pKill" + to_string(pKill); 
	filename += "_pMute" + to_string(pMute); 
    filename += "_pInit" + to_string(Npop_INITIAL);
 	filename += "_L" + to_string(L);
	filename +=".csv";

    filename = path + filename;
    ofstream pop_file; pop_file.open (filename.c_str());

    pop_file << "generation,Npop,diversity,encountered,core_pop,core_size" << endl;

    do{
		list<Species>::iterator sID = kill();       //choose an individual and kill with prob pkill
        if(Npop == 0){ break; }                     //if population totally killed off, end
		if(sID == ecology.end()) sID = choose();	//if we killed the individual, choose another one
		if( mt_rand() < poff(sID) ){				//individual reproduces with probability poff
			asexual(sID);		                    //reproduce asexually
		}
		++t; //counter

		
		if(t >= lgen){	//generation is over
			t = 0; ++t_gens; lgen = Npop / pKill; //recalculate generation length
			print_file(pop_file);		
		}
		
	}while(t_gens < MAX_GENS); //stopping condition. run for MAX_GENS
	pop_file.close();  //Closes the file stream.
	//core_file.close();

	return 0;	

}