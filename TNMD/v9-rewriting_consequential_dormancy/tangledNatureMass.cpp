///////////////////
//  Includes/ Imports
///////////////////

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
#include <sys/stat.h>
#include <stdio.h>
#include <bitset>
#include <stdlib.h>
#include <random>
#include <list>
#include <unordered_set>


////////////////////
//  Definitions
////////////////////

#define L 20         // Genome length
#define N 1048576    // 2^L, number of possible species
#define barwidth 70


/////////////////////
//  Namespaces
/////////////////////

using namespace std;



/////////////////////
//  Parameters
/////////////////////

int generation = 0;               // Current generational number
const int MAX_GENS = 5000;        // Maximum number of generations


const double C = 100;             // Scaling parameter for interspecies interactions


const double pKill_Acti = 0.20;   // Probability of killing an active individual
const double pKill_Dorm = 0.010;  // Probability of killing a dormant individual

const double pMutate = 0.01;      // Probability of mutating

const double tDormant = 0.10;     // Threshold "pOff" for individual to become dormant (not implemented) ¹
const double tActive = 0.40;      // Threshold "pOff" for individual to become active (not implemented) ²

const double pDormant = 0.01;     // Probability of individual becoming dormant ²
const double pActive = 0.50;      // Probability of individual becoming active ²

const double muDormant = 0.005;   // Scaling factor of dormant individual's pressure on resources ²
const double muActive = 0.10;     // Scaling factor of active individual's pressure on resources ²

// ¹ (Has no effect if >= 1.0)
// ² (Has no effect if <= 0.0)

const int INITIAL_POPULATION = 100;      // Initial number of individuals in the ecology

int total_Population = 0;         // Initial population added before species are created
int active_Population = 0;
int dormant_Population = 0;


const double theta = 0.25;        // Density of non-zero interspecies ineteractions

bool interaction[N];              // Array to determine if two species iteract
double alpha[N];                  // First array to determine effect of interatcion between two species
double beta[N];                   // Second array to determine effect of interatcion between two species



/////////////////
//  Random Number Generator
/////////////////

mt19937 mt_generator;  //Mersenne twister engine

double mt_normalise = 1.0 / (double)( 1.0 + mt_generator.max());

inline double mt_rand(){
    return (double)mt_generator()*mt_normalise;
}

inline double mt_rand_Sym(){
    return 2.0f*mt_generator()*mt_normalise - 1.0;
}

inline int random_Species(){
    return (int)(N * mt_rand());
}



///////////////////////
//  Species Object
///////////////////////


class Species {
public:
	int species_ID;			//species ID
	bitset<L> binary_Species_ID;	//binary genome - sID in binary
	int total_population = 0;
	int active_population = 0; //number of individuals non-dormant belonging this species
	int dormant_population = 0; //number of individuals dormant belonging this species 
	
	//initialise
	Species(int species_ID_, int pop_) : species_ID(species_ID_), active_population(pop_), total_population(pop_) {
		bitset<L> tmp (species_ID);
		binary_Species_ID = tmp;

        total_Population += pop_;
        active_Population += pop_;
	}	

    void goDormant(){
        this->active_population--;
        this->dormant_population++;

        active_Population--;
        dormant_Population++;
    }

    void becomeActive(){
        this->active_population++;
        this->dormant_population--;

        active_Population++;
        dormant_Population--;
    }

    void addIndividual(){
        this->total_population++;
        this->active_population++;

        total_Population++;
        active_Population++;
    }

    void killDormant(){
        this->dormant_population--;
        this->total_population--;

        dormant_Population--;
        total_Population--;
    }

    void killActive(){
        this->active_population--;
        this->total_population--;

        active_Population--;
        total_Population--;
    }
};


list<Species> ecology;  //List of extant species
unordered_set<int> encountered; //List of all encountered species


list<Species>::iterator searchNode(list<Species> &ecology, int n) {
	for (list<Species>::iterator current = ecology.begin(); current != ecology.end(); ++current){
		if(current->species_ID == n) return current;
	}
	return ecology.end();
}


/////////////////////////
//  Initialisation
/////////////////////////

inline void initialiseModel( int model_Seed){

    default_random_engine generator(model_Seed + 127);
    mt_generator = mt19937(model_Seed);

    double oC = sqrt(C);

    normal_distribution<double> distribution(0.0, 1.0);
    for (int i = 0; i < N; i++){
        (mt_rand() < theta) ? interaction[i] = true : interaction[i] = false;
        alpha[i] = oC * distribution( generator );
        beta[i] = oC * distribution( generator );
    }

    ecology.clear();

    int random_specie = random_Species();
    encountered.insert(random_specie);
    ecology.emplace_front(random_specie, INITIAL_POPULATION);

}



//////////////////////////
//  Finding pOff
//////////////////////////

inline double calculate_HI(list<Species>::iterator element){
    double sum = 0;

    // Looping over each species in the ecology (excluding self-interaction)
    // Uses the XOR of the binary IDs to reduce the space complexity of the program
    for (list<Species>::iterator current = ecology.begin(); current != ecology.end(); ++current){
        if(current->binary_Species_ID != element->binary_Species_ID){
            bitset<L> intersection = current->binary_Species_ID ^ element->binary_Species_ID;
            long int int_inter = intersection.to_ulong();
            if( interaction[int_inter] ){
                sum += alpha[int_inter] * beta[current->species_ID] * current->active_population;
            }
        }
    }

    return sum;
}


inline double find_H(list<Species>::iterator element){
    return calculate_HI(element)/active_Population - ((muActive * active_Population) + (muDormant * dormant_Population));
}


inline double pOff(list<Species>::iterator elem){
    return 1.0/(1.0 + exp(-find_H(elem)));
}


///////////////////////////
//  Selection
///////////////////////////

inline list<Species>::iterator select_Active(){
    double randPop = mt_rand() * active_Population;
    int sum = 0;
    for (list<Species>::iterator current = ecology.begin(); 
        current != ecology.end(); ++current){
        sum += current->active_population;
        if(randPop <= sum){
            return current;
        }
    }

    // Used for error handling. Was an issue where in the true value of active individuals drift from the value
    // store in `active_Population`, causing issues where the randomly selected individual didn't exist.
    cerr << "WARNING:: NO ACTIVE INDIVIDUAL SELECTED" << endl;
    cerr << "          ACTIVE POPULATION:  " << active_Population << endl;
    cerr << "          DORMANT POPULATION:  " << dormant_Population << endl;
    cerr << "          TOTAL POPULATION:  " << total_Population << endl;
    cerr << "          TARGET:  " << randPop << endl;
    cerr << "          FOUND SUM:  " << sum << endl;
    cerr << "          GENERATION:  " << generation << endl;
    exit(1);
}

inline list<Species>::iterator select_Dormant(){
    double randPop = mt_rand() * dormant_Population;
    int sum = 0;
    for (list<Species>::iterator current = ecology.begin(); 
        current != ecology.end(); ++current){
        sum += current->dormant_population;
        if(randPop <= sum){
            return current;
        }
    }

    cerr << "WARNING:: NO DORMANT INDIVIDUAL SELECTED" << endl;
    exit(1);
}


///////////////////////////
//  Mutation
///////////////////////////

inline bitset<L> mutateOffspring(list<Species>::iterator elem){
    bitset<L> bin_new;
    for(int i=0; i<L; i++){
		(mt_rand() < pMutate) ? bin_new[i] = !elem->binary_Species_ID[i]:
        bin_new[i] = elem->binary_Species_ID[i];  // Each bit in the binary string can mutate with probability pMutate 					
	}
    return bin_new;
}   


///////////////////////////
//  Reproduction
///////////////////////////

inline void asexual(list<Species>::iterator individual){

    individual->killActive();
	

	bitset<L> offspring_A = mutateOffspring(individual);	//new individual genomes
    bitset<L> offspring_B = mutateOffspring(individual);

	if( offspring_A != individual->binary_Species_ID ){ //if new species
		list<Species>::iterator temp_Node = searchNode(ecology, offspring_A.to_ulong() ); //have we seen this species already?
		if(temp_Node == ecology.end()){ //if new species not on list
			ecology.emplace_front(offspring_A.to_ulong(), 1); //add to lsit
			encountered.insert(offspring_A.to_ulong());
		} else { //if new species already on list
			temp_Node->addIndividual();
		}	
	} else {  //no mutation
		individual->addIndividual(); //increase original species count by 1
	}

    if( offspring_B != individual->binary_Species_ID ){ //if new species
		list<Species>::iterator temp_Node = searchNode(ecology, offspring_B.to_ulong() ); //have we seen this species already?
		if(temp_Node == ecology.end()){ //if new species not on list
			ecology.emplace_front(offspring_B.to_ulong(), 1); //add to lsit
			encountered.insert(offspring_B.to_ulong());
		} else { //if new species already on list
			temp_Node->addIndividual();
		}	
	} else {  //no mutation
		individual->addIndividual(); //increase original species count by 1
	}

}


///////////////////////////
//  Death
///////////////////////////

inline list<Species>::iterator kill_Active(list<Species>::iterator individual){
    if(mt_rand() < pKill_Acti) {
        individual->killActive();

        if(individual->total_population == 0){
            ecology.erase(individual);
        }

        return ecology.end(); // Have killed the individual, need to select another.
    }else{
        return individual;
    }
}

inline list<Species>::iterator kill_Dormant(list<Species>::iterator individual){
    if(mt_rand() < pKill_Dorm) {
        individual->killDormant();

        if(individual->total_population == 0){
            ecology.erase(individual);
        }

        return ecology.end(); // Have killed the individual, need to select another.
    }else{
        return individual;
    }
}


///////////////////////////
// Consequential Dormancy
///////////////////////////

// `go_Dormant` determines whether an individual goes dormant or not based on current knowledge.
// Below are several different versions, to allow for adjustment to the rates


// This version inverts the reproduction formula, in essence giving a 50% chance of going dormant
// bool go_Dormant(list<Species>::iterator individual, double pOff, double rand){
//     return (rand < (1 - pOff));
// }
// string dormancy_Type(){
//     return "Default";
// }


// Keeps same slope, reduces range of possile values
// bool go_Dormant(list<Species>::iterator individual, double pOff, double rand){
//     return (rand < (pDormant - pOff));
// }
// string dormancy_Type(){
//     return "ReducedIntersect";
// }


// Reduced Intersect, Increased Slope
bool go_Dormant(list<Species>::iterator individual, double pOff, double rand){
    return ((pDormant * rand) < ((pDormant * pDormant) - pOff));
}
string dormancy_Type(){
    return "RIIS";
}


// Increase slope, and cut off value is sooner
// bool go_Dormant(list<Species>::iterator individual, double pOff, double rand){
//     return (pDormant * rand < (pDormant - pOff));
// }

// Descreases slope but similar cutoff point
// bool go_Dormant(list<Species>::iterator individual, double pOff, double rand){
//     return (rand < pDormant * (1 - pOff));
// }



// `go_Active` determines whether an individual becomes active again or not based on current knowledge.
// Below are several different versions, to allow for adjustment to the rates

// Imitates the same process as reproduction. If it suceeds here, it is likely able to reproduce.
// bool go_Active(list<Species>::iterator individual, double pOff, double rand){
//     return (rand < pOff);
// }

// Is slightly easier to achieve than actually reproducing but signals that the individual has a good chance of reproducing
// if it was active.
bool go_Active(list<Species>::iterator individual, double pOff, double rand){
    return ((pActive * rand) < pOff);
}

///////////////////////////
//  Miscellaneous
///////////////////////////

inline void print_To_File(ofstream &pop_file, double percent=0.05){ //percent = condition for being in the core
	
	int max = 0; 
	int diversity = 0;
	for (list<Species>::iterator cur=ecology.begin(); cur != ecology.end(); ++cur){
		if( cur->active_population >  max ){ max = cur->active_population; }
		++diversity;
	}

	int core_pop = 0;
	int core_size = 0;
	for (list<Species>::iterator cur=ecology.begin(); cur != ecology.end(); ++cur){
		if( cur->active_population > percent * (double) max ){ 
			core_pop += cur->active_population;
			++core_size;
		}
	}

	
	//generation number   number of individuals, awake, asleep 		   number of extant species,  number of species seen, individuals in core,   species in core
	pop_file << generation << "," << total_Population << "," << active_Population << "," << dormant_Population << "," << diversity << "," << encountered.size() << "," << core_pop << "," << core_size << endl;			

}


inline void print_Progress_Bar(double progress){
    cout << "[";
    int pos = progress * barwidth;
   	for (int i = 0; i < barwidth; ++i) {
        if (i < pos) cout << "=";
        else if (i == pos) cout << ">";
        else cout << " ";
    }
    cout << "] " << int(progress * 100.0) << " %\r";
    cout.flush();	
}

void garbage_Collection(list<Species>::iterator next = ecology.begin()){
    for (list<Species>::iterator species = next; species != ecology.end(); ++species){
        if (species->total_population == 0){
            next = ecology.erase(species);
            garbage_Collection(next);
            return;
        }
    }
}


int find_True_Active_Population(){
    int sum = 0;
    for (list<Species>::iterator cur=ecology.begin(); cur != ecology.end(); ++cur){
		    sum += cur->active_population;
	}
    return sum;
}


//////////////////////////
//        Main
//////////////////////////

int main(int argc, char *argv[]){
    int iterations;
    int it = 0;
    int seed;
    string path;
	bool verbose = false;

    // The following checks for additional inputs and sets the seed and path accordingly
    if(argc < 3)
    {
        cerr << "No number of iterations or Path Inputted, Assuming Default (10, .\\)" << endl;

        iterations = 10;
        seed = 12345;
        path = "";
    }
    else
    {
        if (argc > 3){
            verbose = true;
        }        
        iterations = atoi(argv[1]);
        
        path = argv[2]; 
    }

    
    string folderName = path + "massDorm";
    // ADD HYPERPARAMETER VALUES HERE
    folderName += "_InitPop" + to_string(INITIAL_POPULATION);
    folderName += "_killActive" + to_string(pKill_Acti);
    folderName += "_killDormant" + to_string(pKill_Dorm);
    folderName += "_pDormant" + to_string(pDormant);
    folderName += "_muActive" + to_string(muActive);
    folderName += "_muDormant" + to_string(muDormant);
    folderName += "_pMutate" + to_string(pMutate);
    folderName += "_theta" + to_string(theta);
    folderName += "_dormancyType" + dormancy_Type();
    
    const char *fName = folderName.c_str();

    mkdir(fName, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);


    for (int it = 0; it < iterations; it++){
        seed = 123*it + 12345;

        initialiseModel(seed);

        // Following used to check initialisation has worked correctly
        //cerr << "Total Pop: " << total_Population << endl;
        //cerr << ecology.begin()->species_ID << endl << ecology.begin()->total_population << endl << ecology.begin()->active_population;

        int t = 0;
        double generation_Length = active_Population / pKill_Acti;

        ofstream population_File;

        string filename = folderName + "/" + to_string(it) + ".csv";
        population_File.open(filename.c_str());

        population_File << "generation,Npop,Apop,Spop,diversity,encountered,core_pop,core_size" << endl;

        do{
            list<Species>::iterator current;
            if(active_Population > 0){
                current = select_Active();
                current = kill_Active(current);
            }

            if(active_Population > 0){
                if (current == ecology.end()) current = select_Active(); 

                double current_pOff = pOff(current);
                double rand = mt_rand();

                if(rand < current_pOff){
                    asexual(current);
                }else if(go_Dormant(current, current_pOff, rand)){
                    current->goDormant();
                }
            }

            if(dormant_Population > 0){
                current = select_Dormant();
                current = kill_Dormant(current);
            }


            if(dormant_Population > 0){
                if (current == ecology.end()) current = select_Dormant(); 

                double current_pOff = pOff(current);
                double rand = mt_rand();

                if(rand < current_pOff){
                    current->becomeActive();
                }
            }

            t++;        

            if (verbose){
                int sum = 0;
                for (list<Species>::iterator cur=ecology.begin(); cur != ecology.end(); ++cur){
                    sum += cur->active_population;
                }
                cout << "\nSUBGEN: " << t <<  ", ACTIVE POP: " << active_Population << ", ACTUAL" << sum << endl;
            }

            if(t >= generation_Length){	//generation is over
                t = 0; ++generation;
                garbage_Collection();
                generation_Length = active_Population / pKill_Acti; //recalculate generation length
                print_To_File(population_File);

                print_Progress_Bar(float(generation) / float(MAX_GENS));    			
            }

        }while(generation < MAX_GENS);
        population_File.close();
    }
    return 0; // Complete with no errors
}