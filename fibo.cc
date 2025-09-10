#include <iostream>
#include <gmp.h>
#include <chrono>
#include <cstring>

void fib(mpz_t result, unsigned long long n) {
    mpz_t a, b;
    mpz_inits(a, b, nullptr);

    mpz_set_ui(a, 0); // F(0)
    mpz_set_ui(b, 1); // F(1)

    for (int i = 63 - __builtin_clzll(n); i >= 0; --i) {
        // temp1 = a * (2*b - a)   → F(2k)
        mpz_t t1, t2;
        mpz_inits(t1, t2, nullptr);

        mpz_mul_2exp(t1, b, 1);   // 2*b
        mpz_sub(t1, t1, a);       // 2*b - a
        mpz_mul(t1, a, t1);       // a * (2*b - a)

        // t2 = a^2 + b^2          → F(2k+1)
        mpz_mul(t2, a, a);        // a^2
        mpz_addmul(t2, b, b);     // + b^2

        if ((n >> i) & 1) {
            mpz_set(a, t2);       // a = F(2k+1)
            mpz_add(b, t1, t2);   // b = F(2k) + F(2k+1) = F(2k+2)
        } else {
            mpz_set(a, t1);       // a = F(2k)
            mpz_set(b, t2);       // b = F(2k+1)
        }

        mpz_clears(t1, t2, nullptr);
    }

    mpz_set(result, a);
    mpz_clears(a, b, nullptr);
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
    "F(" << n << ") = " <<
#endif
    str << std::endl;
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
