#include <iostream>
#include <cmath>

#include "src/bucket_quadtree.h"
#include "src/direct_search.h"
#include "src/utilities/utest.h"
#include "src/utilities/point_reader.hpp"
#include "src/utilities/timer.hpp"
#include "src/utilities/mpl_writer.hpp"

UTEST_STATE();

void test_data();
void test_random_set();
void test_query();
void test_bucket_capacity();

int main(const int argc, const char* const argv[])
{
    // check if there is any argument
    if (argc > 1)
    {
        return utest_main(argc, argv);
    }
    test_data();
    test_random_set();
    test_query();
    test_bucket_capacity();
}

void test_data()
{
    std::cout << "Test for swe.csv" << std::endl;
    auto points = sf::readCsvPoints<alg::Point>("test_data/swe.csv");
    alg::Point p1{2e5, 6e6};
    alg::Point p2{5e5, 7e6};
    alg::Rectangle rect{p1, p2};
    // quadtree
    alg::QuadTree root(points, 50);
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
    std::vector<alg::Point> result{};
    root.query(rect, result);
    sf::MplWriter<alg::Point, alg::Rectangle> writer_query("plot_query.py");
    writer_query << points;
    writer_query << rect;
    writer_query << result;
}

void test_random_set()
{
    std::cout << "Test for different random set" << std::endl;
    sf::RandomPointGenerator<alg::Point> generator;
    // from 10 to 10000000 points
    for (int i = 1; i < 8; i++)
    {
        std::cout << "Points: " << std::pow(10, i) << std::endl;
        generator.addNormalPoints(std::pow(10, i));
        auto points = generator.takePoints();
        sf::Timer timer_tree;
        timer_tree.start();
        alg::QuadTree root(points, 10);
        timer_tree.stop();
    }
}

void test_query(const std::vector<alg::Point>& points, const alg::Rectangle& rect, int bucket_capacity)
{
    // quadtree
    alg::QuadTree root(points, bucket_capacity);
    std::vector<alg::Point> result{};
    sf::Timer timer_tree;
    timer_tree.start();
    root.query(rect, result);
    timer_tree.stop();

    // direct search
    alg::DirectSearch qt(points);
    // qt.divide(bucket_capacity);
    sf::Timer timer_direct;
    timer_direct.start();
    std::vector<alg::Point> result_direct = qt.query(rect);
    timer_direct.stop();
}

void test_query()
{
    std::cout << "Test for query performance comparing with direct search" << std::endl;
    sf::RandomPointGenerator<alg::Point> generator;
    // from 10 to 10000000 points
    for (int i = 1; i < 8; i++)
    {
        std::cout << "Points: " << std::pow(10, i) << std::endl;
        generator.addNormalPoints(std::pow(10, i));
        auto points = generator.takePoints();
        alg::Point p1{0, 0};
        alg::Point p2{10, 10};
        alg::Rectangle rect{p1, p2};
        test_query(points, rect, 10);
    }
}

void test_bucket_capacity()
{
    std::cout << "Test for different bucket capacity" << std::endl;
    sf::RandomPointGenerator<alg::Point> generator;
    generator.addNormalPoints(1000000);
    auto points = generator.takePoints();
    alg::Point p1{0, 0};
    alg::Point p2{10, 10};
    alg::Rectangle rect{p1, p2};
    // from 1 to 100000 bucket capacity
    for (int i = 0; i < 6; i++)
    {
        std::cout << "Bucket capacity: " << std::pow(10, i) << std::endl;
        sf::Timer timer_tree;
        timer_tree.start();
        alg::QuadTree root(points, std::pow(10, i));
        std::vector<alg::Point> result{};
        root.query(rect, result);
        timer_tree.stop();
    }
}
