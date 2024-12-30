//
// Created by Yunhao Xu on 24-12-30.
//

#ifndef SDE_H
#define SDE_H

#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <fstream>
#include <thread>

#include "utilities/utest.h"

namespace alg
{
    /**
     * Class for simulating a stochastic differential equation.
     */
    class StochasticSimulation
    {
        const double b;
        const double dt;
        const double max_time;
        const int M;

    public:
        StochasticSimulation(const double b, const double dt, const double max_time, const int M)
            : b(b), dt(dt), max_time(max_time), M(M)
        {
        }

        /**
         * Simulate one path and return extinction time
         * @param local_rng The random number generator
         * @return The extinction time or the maximum number of steps
         */
        [[nodiscard]] int simulate_path(std::mt19937_64& local_rng) const
        {
            std::gamma_distribution gamma_dist{2.0, 1.0 / b};
            std::normal_distribution normal_dist{0.0, dt};

            double X = gamma_dist(local_rng);

            const int max_steps = static_cast<int>(max_time / dt);
            for (int n = 0; n < max_steps; ++n)
            {
                if (X <= 0)
                {
                    return n;
                }

                // Euler-Maruyama update
                const double dW = std::sqrt(dt) * normal_dist(local_rng);
                X += -b * dt + dW;
            }

            return max_steps;
        }

        /**
         * Estimate the survival probability
         * @return The survival probability
         */
        [[nodiscard]] std::vector<double> estimate_survival_prob() const
        {
            std::mt19937_64 local_rng(std::random_device{}());
            const int max_steps = static_cast<int>(max_time / dt);
            std::vector<int> extinction_times(M);

            for (int m = 0; m < M; ++m)
            {
                extinction_times[m] = simulate_path(local_rng);
            }

            std::vector survival_prob(max_steps, 0.0);
            for (int t = 0; t < max_steps; ++t)
            {
                int count = 0;
                for (int m = 0; m < M; ++m)
                {
                    if (extinction_times[m] > t)
                    {
                        ++count;
                    }
                }
                survival_prob[t] = static_cast<double>(count) / M;
            }

            return survival_prob;
        }

        /**
         * Estimate the survival probability in parallel
         * @param num_threads The number of threads
         * @return The survival probability
         */
        [[nodiscard]] std::vector<double> estimate_survival_prob_parallel(const int num_threads) const
        {
            const int max_steps = static_cast<int>(max_time / dt);
            std::vector<int> extinction_times(M);

            auto worker = [&](const int start, const int end) {
                std::mt19937_64 local_rng(std::random_device{}());
                for (int m = start; m < end; ++m) {
                    extinction_times[m] = simulate_path(local_rng);
                }
            };

            std::vector<std::thread> threads;
            const int chunk_size = M / num_threads;
            for (int i = 0; i < num_threads; ++i) {
                int start = i * chunk_size;
                int end = i == num_threads - 1 ? M : start + chunk_size;
                threads.emplace_back(worker, start, end);
            }

            for (auto& t : threads) {
                t.join();
            }

            std::vector survival_prob(max_steps, 0.0);
            for (int t = 0; t < max_steps; ++t) {
                int count = 0;
                for (int m = 0; m < M; ++m) {
                    if (extinction_times[m] > t) {
                        ++count;
                    }
                }
                survival_prob[t] = static_cast<double>(count) / M;
            }

            return survival_prob;
        }


        /**
         * Save results to a file
         * @param survival_prob The survival probability
         * @param filename The name of the file
         */
        void save_results(const std::vector<double>& survival_prob, const std::string& filename) const
        {
            std::ofstream file(filename);
            if (!file.is_open())
            {
                std::cerr << "Error opening file: " << filename << std::endl;
                return;
            }

            for (size_t i = 0; i < survival_prob.size(); ++i)
            {
                file << static_cast<double>(i) * dt << "," << survival_prob[i] << "\n";
            }

            file.close();
            std::cout << "Results saved to " << filename << std::endl;
        }
    };
}

#endif //SDE_H

UTEST(SDE, single_threaded)
{
    constexpr double b = 1.0;
    constexpr double dt = 0.001;
    constexpr double max_time = 10.0;
    constexpr int M = 10000;

    const alg::StochasticSimulation simulation(b, dt, max_time, M);

    const std::vector<double> survival_prob = simulation.estimate_survival_prob();

    simulation.save_results(survival_prob, "survival_curve_single.csv");
}

UTEST(SDE, multi_threaded)
{
    constexpr double b = 1.0;
    constexpr double dt = 0.001;
    constexpr double max_time = 10.0;
    constexpr int M = 10000;
    constexpr int num_threads = 4;

    const alg::StochasticSimulation simulation(b, dt, max_time, M);

    const std::vector<double> survival_prob = simulation.estimate_survival_prob_parallel(num_threads);

    simulation.save_results(survival_prob, "survival_curve_parallel.csv");
}
