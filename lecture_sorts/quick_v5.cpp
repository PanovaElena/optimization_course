#include <iostream>
#include <random>
#include <sstream>
#include <chrono>
#include <stack>

void print(float* a, int left, int right);
int batchSize = 16;

__declspec(noinline) void insertsort(float* a, int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        float v = a[i];
        
        int index = left;
        while (index <= i - 1 && a[index] <= v) index++;
        
        for (int j = i - 1; j >= index; j--)
            a[j + 1] = a[j];
        a[index] = v;
    }
}

__declspec(noinline) int partition(float* a, int left, int right) {
    int i = left - 1;
    int j = right;
    const float pivot = a[right];
    
    while (true) {
        while (a[++i] < pivot);
        while (pivot < a[--j])
            if (j == left) break;
        if (i >= j) break;
        std::swap(a[i], a[j]);
    }
    
    std::swap(a[i], a[right]);
    return i;
}

__declspec(noinline) void sort(float* a, int left, int right) {
    if (right - left < batchSize) {
        insertsort(a, left, right);
        return;
    }
    int i = partition(a, left, right);
    if (i - left < right - i) {
        sort(a, left, i-1);
        sort(a, i+1, right);
    }
    else {
        sort(a, i+1, right);
        sort(a, left, i-1);
    }
}


__declspec(noinline) void initializeRandom(float* a, const int n) {
    std::mt19937 gen(0);
    std::uniform_real_distribution<> dis(0.0, 1.0);
    for (int i = 0; i < n; i++)
        a[i] = dis(gen);
}

__declspec(noinline) bool checkRandom(float* a, const int n) {
    for (int i = 0; i < n - 1; i++)
        if (a[i] > a[i + 1]) return false;
    return true;
}

__declspec(noinline) void initializeInversed(float* a, const int n) {
    for (int i = 0; i < n; i++)
        a[i] = n - i - 1;
}

__declspec(noinline) bool checkInversed(float* a, const int n) {
    for (int i = 0; i < n - 1; i++)
        if (a[i] != i) return false;
    return true;
}

__declspec(noinline) void initializeDirect(float* a, const int n) {
    for (int i = 0; i < n; i++)
        a[i] = i;
}

__declspec(noinline) bool checkDirect(float* a, const int n) {
    for (int i = 0; i < n - 1; i++)
        if (a[i] != i) return false;
    return true;
}

__declspec(noinline) void print(float* a, const int n) {
    for (int i = 0; i < n; i++)
        std::cout << a[i] << std::endl;
    std::cout << std::endl;
}

__declspec(noinline) void runDirect(size_t n) {
    float * a = new float[n];
    
    initializeDirect(a, n);   
    const std::chrono::time_point<std::chrono::system_clock> t1 = std::chrono::system_clock::now();  
    sort(a, 0, n-1);  
    const std::chrono::time_point<std::chrono::system_clock> t2 = std::chrono::system_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    bool res = checkDirect(a, n);
    
    delete[] a;
    std::cout << "direct " << res << " " << time << " ";
}

__declspec(noinline) void runInversed(size_t n) {
    float * a = new float[n];
    
    initializeInversed(a, n);   
    const std::chrono::time_point<std::chrono::system_clock> t1 = std::chrono::system_clock::now();  
    sort(a, 0, n-1);  
    const std::chrono::time_point<std::chrono::system_clock> t2 = std::chrono::system_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    bool res = checkInversed(a, n);
    
    delete[] a;
    std::cout << "inversed " << res << " " << time << " ";
}

__declspec(noinline) void runRandom(size_t n) {
    float * a = new float[n];
    
    initializeRandom(a, n);   
    const std::chrono::time_point<std::chrono::system_clock> t1 = std::chrono::system_clock::now();  
    sort(a, 0, n-1);  
    const std::chrono::time_point<std::chrono::system_clock> t2 = std::chrono::system_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    bool res = checkRandom(a, n);
    
    delete[] a;
    std::cout << "random " << res << " " << time << " ";
}


int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Check args" << std::endl;
        return 0;
    }
    
    std::stringstream ss;
    ss << argv[1];
    size_t n;
    ss >> n;
    
    if (argc >= 3) {
        std::stringstream ss;
        ss << argv[2];
        ss >> batchSize;
    }
    
    std::cout << n << " ";
    
    runRandom(n);
    if (n <= 262144) {
        runDirect(n);
        runInversed(n);
    }
    
    std::cout << "(mcs)" << std::endl;
    
}
