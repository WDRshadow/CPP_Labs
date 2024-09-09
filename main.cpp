#include <iostream>
#include <functional>
#include <cmath>

#include "algorithm/ASI.h"

int main()
{
    // define function for x + cos(x^5)
    const std::function f = [](const double x) { return x + cos(pow(x, 5)); };
    constexpr double a = 0;
    constexpr double b = 1;
    // for integrated x + cos(x^5), the integral from 0 to 1 should be 1.45649
    constexpr double tol = 1e-6;
    const asi::ASI asi(f, a, b, tol);
    std::cout << asi.integrate() << std::endl;
}
