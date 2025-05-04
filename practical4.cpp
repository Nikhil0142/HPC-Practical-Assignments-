#include <iostream>
#include <cstdlib>
#include <cuda_runtime.h>

using namespace std;

// CUDA Kernel for vector addition
__global__ void addKernel(int* A, int* B, int* C, int size) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid < size) {
        C[tid] = A[tid] + B[tid];
    }
}

// CUDA Kernel for element-wise vector multiplication
__global__ void mulKernel(int* A, int* B, int* C, int size) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    if (tid < size) {
        C[tid] = A[tid] * B[tid];
    }
}

// Function to initialize array with random numbers
void initialize(int* vector, int size) {
    for (int i = 0; i < size; i++) {
        vector[i] = rand() % 10;
    }
}

// Print first few elements of vector
void print(int* vector, int size) {
    for (int i = 0; i < size; i++) {
        cout << vector[i] << " ";
    }
    cout << endl;
}

// CUDA Error checker
void checkCudaError(cudaError_t err, const char* msg) {
    if (err != cudaSuccess) {
        cerr << "CUDA Error (" << msg << "): " << cudaGetErrorString(err) << endl;
        exit(EXIT_FAILURE);
    }
}

int main() {
    int N = 1000000; // Size of vectors
    size_t bytes = N * sizeof(int);

    // Allocate host memory
    int* A = new int[N];
    int* B = new int[N];
    int* C_add = new int[N];
    int* C_mul = new int[N];

    // Initialize vectors A and B
    initialize(A, N);
    initialize(B, N);

    // Device pointers
    int *d_A, *d_B, *d_C;

    // Allocate device memory
    checkCudaError(cudaMalloc(&d_A, bytes), "cudaMalloc d_A");
    checkCudaError(cudaMalloc(&d_B, bytes), "cudaMalloc d_B");
    checkCudaError(cudaMalloc(&d_C, bytes), "cudaMalloc d_C");

    // Copy data from host to device
    checkCudaError(cudaMemcpy(d_A, A, bytes, cudaMemcpyHostToDevice), "Memcpy A");
    checkCudaError(cudaMemcpy(d_B, B, bytes, cudaMemcpyHostToDevice), "Memcpy B");

    // Define kernel execution parameters
    int threadsPerBlock = 256;
    int blocksPerGrid = (N + threadsPerBlock - 1) / threadsPerBlock;

    // --- Addition Kernel Timing ---
    cudaEvent_t startAdd, stopAdd;
    cudaEventCreate(&startAdd);
    cudaEventCreate(&stopAdd);
    cudaEventRecord(startAdd);

    addKernel<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C, N);

    cudaEventRecord(stopAdd);
    cudaEventSynchronize(stopAdd);
    float timeAdd = 0;
    cudaEventElapsedTime(&timeAdd, startAdd, stopAdd);

    // Copy result back to host
    checkCudaError(cudaMemcpy(C_add, d_C, bytes, cudaMemcpyDeviceToHost), "Memcpy C_add");

    // --- Multiplication Kernel Timing ---
    cudaEvent_t startMul, stopMul;
    cudaEventCreate(&startMul);
    cudaEventCreate(&stopMul);
    cudaEventRecord(startMul);

    mulKernel<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C, N);

    cudaEventRecord(stopMul);
    cudaEventSynchronize(stopMul);
    float timeMul = 0;
    cudaEventElapsedTime(&timeMul, startMul, stopMul);

    checkCudaError(cudaMemcpy(C_mul, d_C, bytes, cudaMemcpyDeviceToHost), "Memcpy C_mul");

    // Display first 10 elements
    cout << "Vector A      : "; print(A, 10);
    cout << "Vector B      : "; print(B, 10);
    cout << "A + B         : "; print(C_add, 10);
    cout << "A * B         : "; print(C_mul, 10);

    // Display timings
    cout << "Time taken for addition kernel     : " << timeAdd << " ms" << endl;
    cout << "Time taken for multiplication kernel: " << timeMul << " ms" << endl;

    // Free host and device memory
    delete[] A;
    delete[] B;
    delete[] C_add;
    delete[] C_mul;
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    return 0;
}
