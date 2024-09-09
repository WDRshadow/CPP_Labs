#include <iostream>
#include <functional>
#include <cmath>

#include "ASI.h"
#include "utest.h"

UTEST_STATE();

void assign_01();

int main(const int argc, const char* const argv[])
{
    // check if there is any argument
    if (argc > 1)
    {
        return utest_main(argc, argv);
    }
    assign_01();
}

void assign_01()
{
    // define function for x + cos(x^5)
    const std::function f = [](const double x) { return x + cos(pow(x, 5)); };
    alg::ASI asi(f, 0, M_PI, 0);
    constexpr double tol[3] = {1e-2, 1e-3, 1e-4};
    for (const double i : tol)
    {
        // counter for number of function evaluations
        int counter = 0;
        std::cout << "Tolerance: " << i << " Integrated value: " << asi.setTol(i).integrate(&counter) << " Counter: " <<
            counter << std::endl;
    }
}
