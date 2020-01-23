#ifdef __CUDACC__
#define tangledCUDA __host__ __device__
#else
#define tangledCUDA
#endif 

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
#include <stdio.h>
#include <bitset>
#include <stdlib.h>
#include <random>
#include <list>
#include <unordered_set>
#include <math.h>
#include <cuda.h>
#include <curand.h>
#include <curand_kernel.h>
#include "cuda_runtime.h"

#define L 20    

