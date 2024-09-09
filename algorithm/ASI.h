//
// Created by Jonathan Hsu on 24-9-9.
//

#ifndef ASI_H
#define ASI_H
#include <functional>

namespace asi {

class ASI {
public:
    ASI(const std::function<double(double)>& f, double a, double b, double tol);
    [[nodiscard]] double integrate() const;

private:
    const std::function<double(double)> f;
    const double a;
    const double b;
    const double tol;
    double total = 0;
    [[nodiscard]] double I(double a, double b) const;
    [[nodiscard]] double I_2(double a, double b) const;
    [[nodiscard]] double _call(double a, double b, double tol) const;
};

} // asi

#endif //ASI_H
