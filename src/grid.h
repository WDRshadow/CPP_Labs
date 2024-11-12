//
// Created by Yunhao Xu on 24-11-7.
//

#pragma once

#include <fstream>
#include <unordered_map>
#include <Eigen/Dense>

#include "asi.h"

#ifndef GRID_H
#define GRID_H

#define X_GRID 0
#define Y_GRID 1

namespace alg
{
    class Point
    {
    public:
        Point(const double x, const double y) : x(x), y(y)
        {
        }

        Point() : x(0.0), y(0.0)
        {
        }

        double x;
        double y;
    };

    class Curve
    {
    public:
        virtual ~Curve() = default;

        /**
         * Interpolate linearly between the points.
         * @param t Interpolation, 0 <= t <= 1
         * @return the point at t
         */
        [[nodiscard]] virtual Point at(double t) const = 0;
    };

    class StraightLine final : public Curve
    {
    public:
        Point endpoint_1;
        Point endpoint_2;

        StraightLine(Point endpoint_1, Point endpoint_2) : endpoint_1(endpoint_1), endpoint_2(endpoint_2)
        {
        }

        [[nodiscard]] Point at(const double t) const override
        {
            return {
                endpoint_1.x + t * (endpoint_2.x - endpoint_1.x),
                endpoint_1.y + t * (endpoint_2.y - endpoint_1.y)
            };
        }
    };

    class EquationCurve : public Curve
    {
    public:
        [[nodiscard]] Point at(const double t) const override
        {
            const double s_total = arc_length(1);
            const auto s_target = t * s_total;
            // search for t_target that make arc_length(t_target) = s_target by using bisection method
            double t_low = 0;
            double t_high = 1;
            double t_mid = 0.5;
            int timer = 0;
            while (std::abs(arc_length(t_mid) - s_target) > tol)
            {
                if (arc_length(t_mid) < s_target)
                {
                    t_low = t_mid;
                }
                else
                {
                    t_high = t_mid;
                }
                t_mid = (t_low + t_high) / 2;
                timer++;
                if (timer > max_iter)
                {
                    break;
                }
            }
            return gamma(t_mid);
        }

        void setDelta(const double delta)
        {
            this->delta = delta;
        }

        void setTol(const double tol)
        {
            this->tol = tol;
        }

        void setMaxIter(const int max_iter)
        {
            this->max_iter = max_iter;
        }

        void setIsCache(const bool isCache)
        {
            this->isCache = isCache;
            if (!isCache)
            {
                cache.clear();
            }
        }

    private:
        int max_iter = 100;
        double delta = 1e-3;
        double tol = 1e-3;
        mutable std::unordered_map<double, double> cache;
        bool isCache = true;
        [[nodiscard]] virtual Point gamma(double t) const = 0;

        [[nodiscard]] virtual Point gammaprime(const double t) const
        {
            if (t <= delta)
            {
                return {
                    (gamma(t + delta).x - gamma(t).x) / delta,
                    (gamma(t + delta).y - gamma(t).y) / delta
                };
            }
            if (t >= 1 - delta)
            {
                return {
                    (gamma(t).x - gamma(t - delta).x) / delta,
                    (gamma(t).y - gamma(t - delta).y) / delta
                };
            }
            return {
                (gamma(t + delta).x - gamma(t - delta).x) / (2 * delta),
                (gamma(t + delta).y - gamma(t - delta).y) / (2 * delta)
            };
        }

        [[nodiscard]] static double norm(const Point& p)
        {
            return std::sqrt(p.x * p.x + p.y * p.y);
        }

        [[nodiscard]] double arc_length(const double t) const
        {
            // check if the value is already in the cache
            if (const auto it = cache.find(t); it != cache.end()) return it->second;
            const auto result = ASI([this](const double x) { return norm(gammaprime(x)); }, 0, t, tol * 1e-1).
                integrate();
            if (isCache) cache[t] = result;
            return result;
        }
    };

    class Domain
    {
    public:
        const Curve& top;
        const Curve& bottom;
        const Curve& left;
        const Curve& right;

        Domain(Curve& top, Curve& bottom, Curve& left, Curve& right) : top(top), bottom(bottom), left(left),
                                                                       right(right)
        {
        }
    };

    class Grid
    {
    public:
        int num_nodes;

        Grid(const Domain& domain, const int num_nodes) : num_nodes(num_nodes)
        {
            Eigen::MatrixXd grid_x(num_nodes, num_nodes);
            Eigen::MatrixXd grid_y(num_nodes, num_nodes);
            const double h = 1.0 / (num_nodes - 1);

            // Set the boundary
            for (int i = 0; i < num_nodes; i++)
            {
                // Top boundary
                const auto point_top_i = domain.top.at(i * h);
                grid_x(num_nodes - 1, i) = point_top_i.x;
                grid_y(num_nodes - 1, i) = point_top_i.y;

                // Bottom boundary
                const auto point_bottom_i = domain.bottom.at(i * h);
                grid_x(0, i) = point_bottom_i.x;
                grid_y(0, i) = point_bottom_i.y;

                // Left boundary
                const auto point_left_i = domain.left.at(i * h);
                grid_x(i, 0) = point_left_i.x;
                grid_y(i, 0) = point_left_i.y;

                // Right boundary
                const auto point_right_i = domain.right.at(i * h);
                grid_x(i, num_nodes - 1) = point_right_i.x;
                grid_y(i, num_nodes - 1) = point_right_i.y;
            }

            // Set the inner points using TFI
            for (int i = 1; i < num_nodes - 1; i++)
            {
                for (int j = 1; j < num_nodes - 1; j++)
                {
                    const double s = static_cast<double>(i) / (num_nodes - 1);
                    const double t = static_cast<double>(j) / (num_nodes - 1);

                    const auto point_x = (1 - s) * grid_x(0, j) + s * grid_x(num_nodes - 1, j) +
                        (1 - t) * grid_x(i, 0) + t * grid_x(i, num_nodes - 1) -
                        ((1 - s) * (1 - t) * grid_x(0, 0) + s * (1 - t) * grid_x(num_nodes - 1, 0) +
                            (1 - s) * t * grid_x(0, num_nodes - 1) + s * t * grid_x(num_nodes - 1, num_nodes - 1));
                    grid_x(i, j) = point_x;

                    const auto point_y = (1 - s) * grid_y(0, j) + s * grid_y(num_nodes - 1, j) +
                        (1 - t) * grid_y(i, 0) + t * grid_y(i, num_nodes - 1) -
                        ((1 - s) * (1 - t) * grid_y(0, 0) + s * (1 - t) * grid_y(num_nodes - 1, 0) +
                            (1 - s) * t * grid_y(0, num_nodes - 1) + s * t * grid_y(num_nodes - 1, num_nodes - 1));
                    grid_y(i, j) = point_y;
                }
            }
            this->grid_x = grid_x;
            this->grid_y = grid_y;
        }

        void write_to_file(const std::string& filename, const int axis) const
        {
            const Eigen::MatrixXd* grid;
            if (axis == X_GRID)
            {
                grid = &grid_x;
            }
            else if (axis == Y_GRID)
            {
                grid = &grid_y;
            }
            else
            {
                throw std::invalid_argument("Invalid axis");
            }
            std::ofstream file;
            file.open(filename);
            for (int i = 0; i < num_nodes; i++)
            {
                for (int j = 0; j < num_nodes; j++)
                {
                    file << (*grid)(num_nodes - 1 - i, j) << " ";
                }
                file << std::endl;
            }
            file.close();
        }

    private:
        Eigen::MatrixXd grid_x;
        Eigen::MatrixXd grid_y;
    };
}

#endif //GRID_H

UTEST(GRID, TEST)
{
    const auto bottomLeft = alg::Point(-10.0, 0.0);
    const auto bottomRight = alg::Point(5.0, 0.0);
    const auto topRight = alg::Point(5.0, 3.0);
    const auto topLeft = alg::Point(-10.0, 3.0);
    auto bottom = alg::StraightLine(bottomLeft, bottomRight);
    auto right = alg::StraightLine(bottomRight, topRight);
    auto top = alg::StraightLine(topRight, topLeft);
    auto left = alg::StraightLine(topLeft, bottomLeft);
    const auto domain = alg::Domain(top, bottom, left, right);
    const auto grid = alg::Grid(domain, 20);
    ASSERT_EQ(grid.num_nodes, 20);
}
