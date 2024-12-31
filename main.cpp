#include <iostream>
#include <cmath>

#include "src/utilities/utest.h"
#include "src/utilities/timer.hpp"

#include "src/sde.h"

UTEST_STATE();

void assign_04();

int main(const int argc, const char* const argv[])
{
    // check if there is any argument
    if (argc > 1)
    {
        return utest_main(argc, argv);
    }
    assign_04();
}

void assign_04()
{
    constexpr double b = 1.0;
    constexpr double dt = 0.001;
    constexpr double max_time = 10.0;
    constexpr int M = 1000000;
    constexpr int num_threads = 4;
    const alg::StochasticSimulation simulation(b, dt, max_time, M);
    sf::Timer timer;
    timer.start("Single-threaded simulation");
    const std::vector<double> survival_prob_single = simulation.estimate_survival_prob();
    timer.stop();
    simulation.save_results(survival_prob_single, "survival_curve_single.csv");
    timer.start("Parallel simulation");
    const std::vector<double> survival_prob_parallel = simulation.estimate_survival_prob_parallel(num_threads);
    timer.stop();
    simulation.save_results(survival_prob_parallel, "survival_curve_parallel.csv");
}

