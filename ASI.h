//
// Created by Jonathan Hsu on 24-9-9.
//

#ifndef ASI_H
#define ASI_H
#include <functional>
#include <cmath>

#include "utest.h"

namespace alg
{
    /**
     * Adaptive Simpson's Rule
     */
    class ASI
    {
    public:
        /**
         * Constructor of Adaptive Simpson's Rule. After construction, call `integrate()` to get the integrated value.
         *
         * @param f: function to integrate
         * @param a: lower bound
         * @param b: upper bound
         * @param tol: tolerance
         */
        ASI(const std::function<double(double)>& f, const double a, const double b,
            const double tol): f(f), a(a), b(b), tol(tol)
        {
        }

        /**
         * integrate function f from a to b
         *
         * @param counter: counter for number of function evaluations
         * @return integrated value
         */
        double integrate(int* counter) const
        {
            checkTol();
            return call(a, b, tol, counter);
        }

        /**
         * integrate function f from a to b
         *
         * @return integrated value
         */
        double integrate() const
        {
            int _ = 0;
            return integrate(&_);
        }

        /**
         * set tolerance
         *
         * @param tol: tolerance
         */
        ASI setTol(const double tol)
        {
            this->tol = tol;
            return *this;
        }

    private:
        const std::function<double(double)> f;
        const double a;
        const double b;
        double tol;

        [[nodiscard]] double I(const double a, const double b, int* counter) const
        {
            *counter += 3;
            return 1.0 / 6 * (b - a) * (f(a) + 4 * f((a + b) / 2) + f(b));
        }

        [[nodiscard]] double I_2(const double a, const double b, int* counter) const
        {
            const double mid = (a + b) / 2;
            return I(a, mid, counter) + I(mid, b, counter);
        }

        [[nodiscard]] double call(const double a, const double b, const double tol, int* counter) const
        {
            const double i_1 = I(a, b, counter);
            const double i_2 = I_2(a, b, counter);
            if (std::abs(i_1 - i_2) < 15 * tol) return i_2;
            const double mid = (a + b) / 2;
            return call(a, mid, tol / 2, counter) + call(mid, b, tol / 2, counter);
        }

        void checkTol() const
        {
            if (tol <= 0)
            {
                throw std::invalid_argument("Tolerance must be positive.");
            }
        }
    };
} // asi

#endif //ASI_H

UTEST(ASI, TEST_1)
{
    const std::function f = [](const double x) { return x; };
    const alg::ASI asi(f, 0, 1, 1e-2);
    EXPECT_EQ(asi.integrate(), 0.5);
}

UTEST(ASI, TEST_TOL)
{
    const std::function f = [](const double x) { return x; };
    const alg::ASI asi(f, 0, 1, 0);
    bool thrown = false;
    try
    {
        asi.integrate();
    }
    catch (std::invalid_argument& _)
    {
        thrown = true;
    }
    EXPECT_TRUE(thrown);
}
