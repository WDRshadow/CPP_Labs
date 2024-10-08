//
// Created by Yunhao Xu on 24-10-8.
//

#ifndef TREE_H
#define TREE_H

#include <vector>
#include <tuple>

#include "utilities/utest.h"
#include "utilities/mpl_writer.hpp"
#include "utilities/random_points.hpp"

namespace alg
{
    class Point
    {
    public:
        Point(double x, double y) : x(x), y(y)
        {
        }

        Point() : x(0.0), y(0.0)
        {
        }

        double x;
        double y;
    };

    class Rectangle
    {
    public:
        Rectangle(Point bottomLeft, Point topRight) : bottomLeft(bottomLeft), topRight(topRight)
        {
        }

        Rectangle() : bottomLeft(Point()), topRight(Point())
        {
        }

        Point bottomLeft;
        Point topRight;
    };

    class QuadTree
    {
    public:
        Rectangle rect;
        std::vector<Point> points;
        std::unique_ptr<QuadTree> topLeft;
        std::unique_ptr<QuadTree> topRight;
        std::unique_ptr<QuadTree> bottomLeft;
        std::unique_ptr<QuadTree> bottomRight;
        int capacity;
        bool isLeaf;

        QuadTree(std::vector<Point>& points, int capacity, Rectangle rect) : points(points), capacity(capacity),
                                                                             rect(rect)
        {
            isLeaf = true;
            topLeft = std::nullptr_t();
            topRight = std::nullptr_t();
            bottomLeft = std::nullptr_t();
            bottomRight = std::nullptr_t();
            this->divide();
        }

        QuadTree(std::vector<Point>& points, int capacity) : points(points), capacity(capacity)
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
            rect = Rectangle(Point(x_min, y_min), Point(x_max, y_max));
            isLeaf = true;
            topLeft = std::nullptr_t();
            topRight = std::nullptr_t();
            bottomLeft = std::nullptr_t();
            bottomRight = std::nullptr_t();
            this->divide();
        }

        QuadTree(const QuadTree& other) : rect(other.rect), points(other.points), capacity(other.capacity),
                                          isLeaf(other.isLeaf)
        {
            if (other.topLeft) topLeft = std::make_unique<QuadTree>(*other.topLeft);
            if (other.topRight) topRight = std::make_unique<QuadTree>(*other.topRight);
            if (other.bottomLeft) bottomLeft = std::make_unique<QuadTree>(*other.bottomLeft);
            if (other.bottomRight) bottomRight = std::make_unique<QuadTree>(*other.bottomRight);
        }

        void query(Rectangle rect, std::vector<Point>& result)
        {
            if (!check_intersect(rect))
            {
                return;
            }
            if (check_include(rect))
            {
                result.insert(result.end(), points.begin(), points.end());
                return;
            }
            if (!isLeaf)
            {
                topLeft->query(rect, result);
                topRight->query(rect, result);
                bottomLeft->query(rect, result);
                bottomRight->query(rect, result);
            }
            else
            {
                const double x_min = rect.bottomLeft.x;
                const double x_max = rect.topRight.x;
                const double y_min = rect.bottomLeft.y;
                const double y_max = rect.topRight.y;
                for (Point point : points)
                {
                    if (point.x >= x_min && point.x <= x_max && point.y >= y_min && point.y <= y_max)
                    {
                        result.push_back(point);
                    }
                }
            }
        }

    private:
        void divide()
        {
            double x_min = rect.bottomLeft.x;
            double x_max = rect.topRight.x;
            double y_min = rect.bottomLeft.y;
            double y_max = rect.topRight.y;
            // check if the node is a leaf
            if (points.size() > capacity)
            {
                isLeaf = false;
                // divide the points into four quadrants
                const double x_mid = (x_min + x_max) / 2.0;
                const double y_mid = (y_min + y_max) / 2.0;
                const Point bl(rect.bottomLeft.x, rect.bottomLeft.y);
                const Point mm(x_mid, y_mid);
                const Point tm(x_mid, rect.topRight.y);
                const Point ml(rect.bottomLeft.x, y_mid);
                const Point mr(rect.topRight.x, y_mid);
                const Point tr(rect.topRight.x, rect.topRight.y);
                const Point bm(x_mid, rect.bottomLeft.y);
                const Rectangle topLeftR(ml, tm);
                const Rectangle topRightR(mm, tr);
                const Rectangle bottomLeftR(bl, mm);
                const Rectangle bottomRightR(bm, mr);
                std::vector<Point> topLeftPoints{};
                std::vector<Point> topRightPoints{};
                std::vector<Point> bottomLeftPoints{};
                std::vector<Point> bottomRightPoints{};
                for (Point point : points)
                {
                    if (point.x >= x_min && point.x <= x_mid && point.y >= y_mid && point.y <= y_max)
                    {
                        topLeftPoints.push_back(point);
                    }
                    else if (point.x >= x_mid && point.x <= x_max && point.y >= y_mid && point.y <= y_max)
                    {
                        topRightPoints.push_back(point);
                    }
                    else if (point.x >= x_min && point.x <= x_mid && point.y >= y_min && point.y <= y_mid)
                    {
                        bottomLeftPoints.push_back(point);
                    }
                    else
                    {
                        bottomRightPoints.push_back(point);
                    }
                }
                topLeft = std::make_unique<QuadTree>(topLeftPoints, capacity, topLeftR);
                topRight = std::make_unique<QuadTree>(topRightPoints, capacity, topRightR);
                bottomLeft = std::make_unique<QuadTree>(bottomLeftPoints, capacity, bottomLeftR);
                bottomRight = std::make_unique<QuadTree>(bottomRightPoints, capacity, bottomRightR);
            }
        }

        [[nodiscard]] bool check_intersect(Rectangle rect) const
        {
            return !(rect.topRight.x < this->rect.bottomLeft.x || rect.bottomLeft.x > this->rect.topRight.x ||
                rect.topRight.y < this->rect.bottomLeft.y || rect.bottomLeft.y > this->rect.topRight.y);
        }

        [[nodiscard]] bool check_include(Rectangle rect) const
        {
            return rect.bottomLeft.x <= this->rect.bottomLeft.x && rect.topRight.x >= this->rect.topRight.x &&
                rect.bottomLeft.y <= this->rect.bottomLeft.y && rect.topRight.y >= this->rect.topRight.y;
        }
    };

    class DirectSearch
    {
    public:
        explicit DirectSearch(std::vector<Point> points) : points(std::move(points))
        {
        }

        std::vector<Point> query(Rectangle rect)
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

#endif //TREE_H

UTEST(QuadTree, Test)
{
    sf::RandomPointGenerator<alg::Point> generator;
    generator.addNormalPoints(32, alg::Point{2.0, 3.0});
    alg::Point p1{0.0, 0.0};
    alg::Point p2{2.0, 3.0};
    alg::Rectangle rect{p1, p2};
    auto points = generator.takePoints();
    alg::QuadTree root(points, 5);
    std::vector<alg::Point> result{};
    root.query(rect, result);
    ASSERT_NEAR(result.size(), 8, 2);
}

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
    ASSERT_NEAR(result.size(), 8, 2);
}
