//
// Created by Jonathan Hsu on 24-9-9.
//

#include "ASI.h"

namespace asi {
    ASI::ASI(const std::function<double(double)>& f, const double a, const double b, const double tol): f(f), a(a), b(b), tol(tol) {}

    double ASI::integrate() const
    {
        return _call(a, b, tol);
    }

    double ASI::_call(double a, double b, double tol) const
    {
        double i_1 = I(a, b);
        double i_2 = I_2(a, b);
        if (std::abs(i_1 - i_2) < 15 * tol) return i_2;
        const double mid = (a + b) / 2;
        return _call(a, mid, tol / 2) + _call(mid, b, tol / 2);
    }


    double ASI::I(const double a, const double b) const
    {
        return 1.0 / 6 * (b - a) * (f(a) + 4 * f((a + b) / 2) + f(b));
    }

    double ASI::I_2(const double a, const double b) const
    {
        const double mid = (a + b) / 2;
        return I(a, mid) + I(mid, b);
    }

} // asi