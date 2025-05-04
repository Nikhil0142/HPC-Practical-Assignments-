#include <iostream>
#include <omp.h>
#include <cstdlib>
#include <ctime>
using namespace std;

// Parallel Bubble Sort using OpenMP
void parallelBubbleSort(int arr[], int n) {
    for (int i = 0; i < n; ++i) {
        int start = i % 2;  // even or odd phase
        #pragma omp parallel for
        for (int j = start; j < n - 1; j += 2) {
            if (arr[j] > arr[j+1]) {
                swap(arr[j], arr[j+1]);
            }
        }
    }
}

// Merge two sorted halves
void merge(int arr[], int l, int m, int r) {
    int n1 = m - l + 1, n2 = r - m;
    int* L = new int[n1];
    int* R = new int[n2];
    
    for (int i = 0; i < n1; ++i) L[i] = arr[l + i];
    for (int i = 0; i < n2; ++i) R[i] = arr[m + 1 + i];
    
    int i = 0, j = 0, k = l;
    
    while (i < n1 && j < n2)
        arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
    
    delete[] L;
    delete[] R;
}

// Parallel Merge Sort using OpenMP
void parallelMergeSort(int arr[], int l, int r) {
    if (l < r) {
        int m = (l + r) / 2;
        #pragma omp parallel sections
        {
            #pragma omp section
            parallelMergeSort(arr, l, m);
            #pragma omp section
            parallelMergeSort(arr, m + 1, r);
        }
        merge(arr, l, m, r);
    }
}

// Generate random array
void generateArray(int arr[], int n) {
    for (int i = 0; i < n; ++i)
        arr[i] = rand() % 10000;
}

int main() {
    const int SIZE = 5000;
    int arr1[SIZE], arr2[SIZE];

    srand(time(0));
    generateArray(arr1, SIZE);
    copy(arr1, arr1 + SIZE, arr2);  // copy for second sort

    double t1 = omp_get_wtime();
    parallelBubbleSort(arr1, SIZE);
    double t2 = omp_get_wtime();
    cout << "Parallel Bubble Sort Time: " << t2 - t1 << " seconds\n";

    t1 = omp_get_wtime();
    parallelMergeSort(arr2, 0, SIZE - 1);
    t2 = omp_get_wtime();
    cout << "Parallel Merge Sort Time:  " << t2 - t1 << " seconds\n";

    return 0;
}

