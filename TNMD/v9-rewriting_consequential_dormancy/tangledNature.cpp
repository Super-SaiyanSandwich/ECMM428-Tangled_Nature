///////////////////
//  Includes/ Imports
///////////////////

#include <random>



////////////////////
//  Definitions
////////////////////

#define L 20         // Genome length
#define N 1048576    // 2^L, number of possible species



/////////////////////
//  Namespaces
/////////////////////

using namespace std;



/////////////////////
//  Parameters
/////////////////////

int t = 0;                        // Current generational number
const int MAX_GENS = 5000;        // Maximum number of generations


const double C = 100;             // Scaling parameter for interspecies interactions


const double pKill_Acti = 0.20;   // Probability of killing an active individual
const double pKill_Dorm = 0.01;   // Probability of killing a dormant individual

const double pMutate = 0.05;      // Probability of mutating

const double tDormant = 0.10;     // Threshold "pOff" for individual to become dormant ¹
const double tActive = 0.40;      // Threshold "pOff" for individual to become active ²

const double pDormant = 0.20;     // Probability of individual becoming dormant ²
const double pActive = 0.50;      // Probability of individual becoming active ²

const double muDormant = 0.00;    // Scaling factor of dormant individual's pressure on resources ²
const double muActive = 0.10;     // Scaling factor of active individual's pressure on resources ²

// ¹ (Has no effect if >= 1.0)
// ² (Has no effect if <= 0.0)

const int Npop_INITIAL = 50;      // Initial number of individuals in the ecology

int population = Npop_INITIAL;    // Setting the initial population
int active_Population = Npop_INITIAL;
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
