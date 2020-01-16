#include <iostream>
#include <math.h>

#include <curand.h>
#include <curand_kernel.h>
#include <stdio.h>

#define N 10485760 //2^20 
#define blocks 1
#define threads 128

__global__
void setup_kernel(curandState *state)
{
  int idx = threadIdx.x+blockDim.x*blockIdx.x;
  curand_init(1234, idx, 0, &state[idx]);
}


__global__ void print_kernel() {
  printf("Hello from block %d, thread %d\n", blockIdx.x, threadIdx.x);
}


// Kernel function to add the elements of two arrays
__global__
void add(int n, float *x, float *y)
{
  int index = threadIdx.x;
  int stride = blockDim.x;

  for (int i = index; i < n; i += stride)
      y[i] = x[i] + y[i];
}



__global__
void rand_Add(curandState *my_curandstate, int n, float *x, float *y)
{
  int index = threadIdx.x;
  int stride = blockDim.x;

  int idx = threadIdx.x + blockDim.x*blockIdx.x;

  float randf;

  for (int i = index; i < n; i += stride)
  {
      randf = curand_normal_double(my_curandstate+idx);
      y[i] = x[i] + y[i] + randf;
  }
}



void print_Dev_Prop(cudaDeviceProp devProp)
{
    printf("Major revision number:         %d\n",  devProp.major);
    printf("Minor revision number:         %d\n",  devProp.minor);
    printf("Name:                          %s\n",  devProp.name);
    printf("Total global memory:           %zu\n",  devProp.totalGlobalMem);
    printf("Total shared memory per block: %zu\n",  devProp.sharedMemPerBlock); 
    printf("Total registers per block:     %d\n",  devProp.regsPerBlock);
    printf("Warp size:                     %d\n",  devProp.warpSize);
    printf("Maximum memory pitch:          %zu\n",  devProp.memPitch);
    printf("Maximum threads per block:     %d\n",  devProp.maxThreadsPerBlock);
    for (int i = 0; i < 3; ++i)
    printf("Maximum dimension %d of block:  %d\n", i, devProp.maxThreadsDim[i]);
    for (int i = 0; i < 3; ++i)
    printf("Maximum dimension %d of grid:   %d\n", i, devProp.maxGridSize[i]);
    printf("Clock rate:                    %d\n",  devProp.clockRate);
    printf("Total constant memory:         %zu\n",  devProp.totalConstMem);
    printf("Texture alignment:             %zu\n",  devProp.textureAlignment);
    printf("Concurrent copy and execution: %s\n",  (devProp.deviceOverlap ? "Yes" : "No"));
    printf("Number of multiprocessors:     %d\n",  devProp.multiProcessorCount);
    printf("Kernel execution timeout:      %s\n",  (devProp.kernelExecTimeoutEnabled ? "Yes" : "No"));
    return;
}


void discover_CUDA_Devices(){
  int devCount;
  cudaGetDeviceCount(&devCount);
  printf("CUDA Device Query...\n");
  printf("There are %d CUDA devices.\n", devCount);


  // Iterate through devices
  for (int i = 0; i < devCount; ++i)
  {
    // Get device properties
    printf("\nCUDA Device #%d\n", i);
    cudaDeviceProp devProp;
    cudaGetDeviceProperties(&devProp, i);
    print_Dev_Prop(devProp);
  }
}


int main(void)
{

  float *x, *y;

  float **z;

  curandState *d_state;


  // Allocate Unified Memory – accessible from CPU or GPU
  cudaMallocManaged(&x, N*sizeof(float));
  cudaMallocManaged(&y, N*sizeof(float));

  cudaMallocManaged(&z, 2*sizeof(float*));

  cudaMallocManaged(&d_state, sizeof(curandState));

  // initialize x and y arrays on the host
  // for (int i = 0; i < N; i++) {
  //   x[i] = 1.0f;
  //   y[i] = 2.0f;
  // }
  std::fill(x,x + N, 1.0f);
  std::fill(y,y + N, 2.0f);

  *(z) = x;
  *(z+1) = y;

  

  setup_kernel<<<blocks, threads>>>(d_state);

  //print_kernel<<<blocks, threads>>>();
  // Run kernel on 1M elements on the GPU over 256 threads
  rand_Add<<<blocks, threads>>>(d_state, N, x, y);

  // Wait for GPU to finish before accessing on host
  cudaDeviceSynchronize();

  std::cout << y[1] << std::endl;
  std::cout << &y[1] << std::endl;
  std::cout << z[1][1] << std::endl;
  std::cout << &z[1][1] << std::endl;
 
  // Check for errors (all values should be 3.0f)
  float maxError = 0.0f;
  float total = 0.0f;

  for (int i = 0; i < N; i++)
  {
    maxError = fmax(maxError, fabs(y[i]-3.0f));
    total += y[i]-3.0f;
  }
  std::cout << "Max error: " << maxError << std::endl;
  std::cout << "Average error:" << (total / N) << std::endl;
  std::cout << N << std::endl;


  // Free memory
  cudaFree(x);
  cudaFree(y);
  
  return 0;
}