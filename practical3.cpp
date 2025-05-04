#include <iostream>    
#include <omp.h>       
#include <climits>     

int main() {
    
    int arr[] = {10, 20, 5, 30, 15, 25, 2, 40};

    
    int n = sizeof(arr) / sizeof(arr[0]);

    
    int sum = 0;
    int min_val = INT_MAX;
    int max_val = INT_MIN;

    
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }

    
    #pragma omp parallel for reduction(min:min_val)
    for (int i = 0; i < n; i++) {
        if (arr[i] < min_val)
            min_val = arr[i];
    }

  
    #pragma omp parallel for reduction(max:max_val)
    for (int i = 0; i < n; i++) {
        if (arr[i] > max_val)
            max_val = arr[i];
    }

    
    float average = static_cast<float>(sum) / n;

    
    std::cout << "Sum     = " << sum << std::endl;
    std::cout << "Average = " << average << std::endl;
    std::cout << "Minimum = " << min_val << std::endl;
    std::cout << "Maximum = " << max_val << std::endl;

    return 0;
}
