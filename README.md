# Bucket quadtrees

A quadtree is a tree where each parent has four children: northwest (NW), northeast (NE), southwest (SW), and southeast (SE), called quadrants. In this section I implement the QuadTree class in a way similar to link-list. Each instance of class QuadTree is a node. Each node, if they are not the leaf node, will point to 4 other nodes representing northwest, northeast, southwest, and southeast. The construction and the query process is a recursive process. The search process includes pruning operations to improve search speed.

## Installation

1. Clone the repository

2. Ensure you have a C++ compiler and `cmake` installed. At least C++17 and `cmake` 3.10 are required.

3. Build the project:
    ```sh
    mkdir build
    cd build
    cmake ..
    make
    ```
   
4. You may also need to move the data-set to the `build` folder:
    ```sh
    mv ../test_data/ ./
    ```

## Usage

To run the main program, execute the following command from the `build` directory:
```sh
./CPP_Labs
```

The main program will read points from the data set `swe.csv` and generate the plot files. Then it will also run the tests function that get the result below.

## Testing

The project includes unit tests to verify if the `QuadTree` and `DirectSearch` class can run. To run the tests, use the following command from the build directory:

```sh
./CPP_Labs --utest
```
