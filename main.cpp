#include <iostream>
#include <functional>
#include <cmath>

#include "src/asi.h"
#include "src/bucket_quadtrees.h"
#include "src/utilities/utest.h"
#include "src/utilities/point_reader.hpp"
#include "src/utilities/timer.hpp"
#include "src/utilities/mpl_writer.hpp"

UTEST_STATE();

void assign_01();
void assign_02();

int main(const int argc, const char* const argv[])
{
    // check if there is any argument
    if (argc > 1)
    {
        return utest_main(argc, argv);
    }
    // assign_01();
    assign_02();
}

void assign_01()
{
    // define function for x + cos(x^5)
    const std::function f = [](const double x) { return x + cos(pow(x, 5)); };
    alg::ASI asi(f, 0, M_PI, 0);
    constexpr double tol[3] = {1e-2, 1e-3, 1e-4};
    for (const double i : tol)
    {
        // counter for number of function evaluations
        int counter = 0;
        std::cout << "Tolerance: " << i << " Integrated value: " << asi.setTol(i).integrate(&counter) << " Counter: " <<
            counter << std::endl;
    }
}

void assign_02()
{
    auto points = sf::readCsvPoints<alg::Point>("test_data/swelakes.csv");
    alg::Point p1{2e5, 6e6};
    alg::Point p2{5e5, 7e6};
    alg::Rectangle rect{p1, p2};
    // quadtree
    alg::QuadTree root(points, 5000);
    sf::MplWriter<alg::Point, alg::Rectangle> writer("plot.py");
    std::vector<alg::QuadTree> queue{};
    queue.push_back(root);
    while (!queue.empty())
    {
        alg::QuadTree current = queue.back();
        queue.pop_back();
        if (!current.isLeaf)
        {
            queue.push_back(*current.topLeft);
            queue.push_back(*current.topRight);
            queue.push_back(*current.bottomLeft);
            queue.push_back(*current.bottomRight);
        }
        else
        {
            writer << current.rect;
            writer << current.points;
        }
    }
    sf::Timer timer;
    timer.start();
    std::vector<alg::Point> result{};
    root.query(rect, result);
    sf::MplWriter<alg::Point, alg::Rectangle> writer_query("plot_query.py");
    writer_query << points;
    writer_query << rect;
    writer_query << result;
    timer.stop();

    // direct search
    alg::DirectSearch qt(points);
    qt.divide(5000);
    sf::MplWriter<alg::Point, alg::Rectangle> writer1("plot1.py");
    for (auto r : qt.getResult())
    {
        writer1 << std::get<0>(r);
        writer1 << std::get<1>(r);
    }
    sf::Timer timer2;
    timer2.start();
    std::vector<alg::Point> result1 = qt.query(rect);
    sf::MplWriter<alg::Point, alg::Rectangle> writer1_query("plot1_query.py");
    writer1_query << points;
    writer1_query << rect;
    writer1_query << result1;
    timer.stop();
}
