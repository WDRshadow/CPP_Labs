//
// Created by Yunhao Xu on 24-10-29.
//

#ifndef DIRECT_SEARCH_H
#define DIRECT_SEARCH_H

#include <vector>
#include <tuple>

#include "utilities/utest.h"
#include "utilities/mpl_writer.hpp"
#include "utilities/random_points.hpp"

namespace alg
{
    class DirectSearch
    {
    public:
        explicit DirectSearch(std::vector<Point> points) : points(std::move(points))
        {
        }

        [[nodiscard]] std::vector<Point> query(const Rectangle& rect) const
        {
            double x_min = rect.bottomLeft.x;
            double x_max = rect.topRight.x;
            double y_min = rect.bottomLeft.y;
            double y_max = rect.topRight.y;
            std::vector<Point> result{};
            for (Point point : points)
            {
                if (point.x >= x_min && point.x <= x_max && point.y >= y_min && point.y <= y_max)
                {
                    result.push_back(point);
                }
            }
            return result;
        }

        void divide(int capacity)
        {
            // calculate the bounding box of the points
            double x_min = points[0].x;
            double x_max = points[0].x;
            double y_min = points[0].y;
            double y_max = points[0].y;
            for (Point point : points)
            {
                x_min = std::min(x_min, point.x);
                x_max = std::max(x_max, point.x);
                y_min = std::min(y_min, point.y);
                y_max = std::max(y_max, point.y);
            }
            Rectangle rect = Rectangle(Point(x_min, y_min), Point(x_max, y_max));
            std::vector<Rectangle> result{};
            std::vector<Rectangle> queue{};
            queue.push_back(rect);
            while (!queue.empty())
            {
                Rectangle current = queue.back();
                queue.pop_back();
                std::vector<Point> pointsInRect = query(current);
                if (pointsInRect.size() > capacity)
                {
                    // divide the rectangle into four sub-rectangles
                    double x_mid = (current.bottomLeft.x + current.topRight.x) / 2.0;
                    double y_mid = (current.bottomLeft.y + current.topRight.y) / 2.0;
                    Point p1(current.bottomLeft.x, current.bottomLeft.y);
                    Point p2(x_mid, y_mid);
                    Point p3(x_mid, current.topRight.y);
                    Point p4(current.bottomLeft.x, y_mid);
                    Point p5(current.topRight.x, y_mid);
                    Point p6(current.topRight.x, current.topRight.y);
                    Point p7(x_mid, current.bottomLeft.y);
                    Rectangle r1(p1, p2);
                    Rectangle r2(p2, p6);
                    Rectangle r3(p4, p3);
                    Rectangle r4(p7, p5);
                    queue.push_back(r1);
                    queue.push_back(r2);
                    queue.push_back(r3);
                    queue.push_back(r4);
                }
                else
                {
                    result.push_back(current);
                }
            }
            for (auto r : result)
            {
                std::vector<Point> pointsInRect = query(r);
                this->result.push_back(std::make_tuple(r, pointsInRect));
            }
        }

        auto getResult()
        {
            return result;
        }

    private:
        std::vector<Point> points{};
        std::vector<std::tuple<Rectangle, std::vector<Point>>> result;
    };
}

#endif //DIRECT_SEARCH_H

UTEST(DirectSearch, Test)
{
    sf::RandomPointGenerator<alg::Point> generator;
    generator.addNormalPoints(32, alg::Point{2.0, 3.0});
    alg::Point p1{0.0, 0.0};
    alg::Point p2{2.0, 3.0};
    alg::Rectangle rect{p1, p2};
    auto points = generator.takePoints();
    alg::DirectSearch qt(points);
    qt.divide(5);
    std::vector<alg::Point> result = qt.query(rect);
}
