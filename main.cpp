#include <iostream>

#include "src/utilities/utest.h"
#include "src/utilities/timer.hpp"
#include "src/grid.h"

UTEST_STATE();

void assign_03();
alg::Grid run_test(int num_elements, const alg::Domain& domain, alg::EquationCurve& bottom);

int main(const int argc, const char* const argv[])
{
    // check if there is any argument
    if (argc > 1)
    {
        return utest_main(argc, argv);
    }
    assign_03();
}

class BottomCurve final : public alg::EquationCurve
{
public:
    [[nodiscard]] static double f(const double x)
    {
        return 1 / (2 * g(x));
    }

    [[nodiscard]] static double g(const double x)
    {
        if (x >= -10 && x < -3)
        {
            return 1 + std::exp(-3 * (x + 6));
        }
        if (x >= -3 && x <= 5)
        {
            return 1 + std::exp(3 * x);
        }
        throw std::invalid_argument("Invalid x");
    }

private:
    [[nodiscard]] alg::Point gamma(const double t) const override
    {
        const auto x = (1 - t) * -10 + t * 5;
        auto y = f(x);
        return {x, y};
    }
};

void assign_03()
{
    const auto bottomLeft = alg::Point(-10.0, 0.0);
    const auto bottomRight = alg::Point(5.0, 0.0);
    const auto topRight = alg::Point(5.0, 3.0);
    const auto topLeft = alg::Point(-10.0, 3.0);
    auto bottom = BottomCurve();
    auto right = alg::StraightLine(bottomRight, topRight);
    auto top = alg::StraightLine(topLeft, topRight);
    auto left = alg::StraightLine(bottomLeft, topLeft);
    const auto domain = alg::Domain(top, bottom, left, right);
    const auto grid = run_test(20, domain, bottom);
    grid.write_to_file("grid_x.txt", X_GRID);
    grid.write_to_file("grid_y.txt", Y_GRID);
    std::cout << "The grid is written to grid_x.txt and grid_y.txt. Please use `python plotdomain.py` to plot the grid."
        << std::endl;
    for (int i = 1; i < 5; i++)
    {
        const int num_elements = static_cast<int>(std::pow(10, i));
        run_test(num_elements, domain, bottom);
    }
}

alg::Grid run_test(const int num_elements, const alg::Domain& domain, alg::EquationCurve& bottom)
{
    std::cout << "With num_elements = " << num_elements << std::endl;
    bottom.setDelta(1.0 / num_elements);
    std::cout << "Time to create grid with cache: ";
    sf::Timer timer;
    bottom.setIsCache(true);
    timer.start();
    const auto grid = alg::Grid(domain, num_elements);
    timer.stop();
    std::cout << "Time to create grid without cache: ";
    sf::Timer timer1;
    bottom.setIsCache(false);
    timer1.start();
    const auto grid1 = alg::Grid(domain, num_elements);
    timer1.stop();
    return grid;
}

