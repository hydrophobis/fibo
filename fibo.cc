#include <iostream>
#include <gmp.h>
#include <chrono>
#include <cstring>

// Define switches:
// #define QUIET_MODE   // Uncomment to suppress all info except final number
// #define PRINT_FULL   // Uncomment to force printing the entire Fibonacci number

void fib(mpz_t result, unsigned long long n) {
    if (n == 0) {
        mpz_set_ui(result, 0);
        return;
    }
    if (n == 1 || n == 2) {
        mpz_set_ui(result, 1);
        return;
    }
    
    size_t estimated_bits = (size_t)(n * 0.694) + 100;
    
    mpz_t a, b, c, d, tmp1, tmp2;
    mpz_inits(a, b, c, d, tmp1, tmp2, nullptr);
    
    mpz_realloc2(a, estimated_bits);
    mpz_realloc2(b, estimated_bits);
    mpz_realloc2(c, estimated_bits);
    mpz_realloc2(d, estimated_bits);
    mpz_realloc2(tmp1, estimated_bits);
    mpz_realloc2(tmp2, estimated_bits);

    mpz_set_ui(a, 0);
    mpz_set_ui(b, 1);

    int start_bit = 63 - __builtin_clzll(n) - 1;
    
    for (int i = start_bit; i >= 0; --i) {
        mpz_mul_2exp(tmp1, b, 1);
        mpz_sub(tmp1, tmp1, a);
        mpz_mul(c, a, tmp1);

        mpz_mul(tmp1, a, a);
        mpz_mul(tmp2, b, b);
        mpz_add(d, tmp1, tmp2);

        if ((n >> i) & 1) {
            mpz_set(a, d);
            mpz_add(b, c, d);
        } else {
            mpz_set(a, c);
            mpz_set(b, d);
        }
    }

    mpz_set(result, a);
    mpz_clears(a, b, c, d, tmp1, tmp2, nullptr);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
#ifndef QUIET_MODE
        std::cerr << "Usage: " << argv[0] << " <n>" << std::endl;
        std::cerr << "Computes the nth Fibonacci number F(n)" << std::endl;
#endif
        return 1;
    }

    unsigned long long n = std::strtoull(argv[1], nullptr, 10);
    
    mpz_t result;
    mpz_init(result);

#ifndef QUIET_MODE
    std::cout << "Computing F(" << n << ")..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
#endif

    fib(result, n);

#ifndef QUIET_MODE
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(end - start).count();
    
    std::cout << "Computation time: " << elapsed << " seconds" << std::endl;
    std::cout << "F(" << n << ") has " << mpz_sizeinbase(result, 10) << " decimal digits" << std::endl;
#endif
    
    char* str = mpz_get_str(nullptr, 10, result);
    size_t len = strlen(str);
    
// silly lookin ahh formatting code
#ifdef PRINT_FULL
    std::cout <<
#ifndef QUIET_MODE 
    "F(" <<
#endif
    n 
#ifndef QUIET_MODE
    << ") = " 
#endif
    << str << std::endl;
#else
    if (len <= 100) {
        std::cout << "F(" << n << ") = " << str << std::endl;
    } else {
        std::cout << "F(" << n << ") = " << std::string(str, 50) << "..." 
                  << std::string(str + len - 50, 50) << std::endl;
    }
#endif
    
    free(str);
    mpz_clear(result);
    
    return 0;
}
