# Structured Grid

This project implements the Grid generator in C++.

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

## Usage

To run the main program, execute the following command from the `build` directory:
```sh
./CPP_Labs
```

The main program will generate a structured grid and write it to a file. The grid is generated using the and will compare the time taken to generate the grid with and without cache.

## Testing

Unit tests are included in the project to verify the functionality of the Grid. To run the tests, execute the following command from the build directory:

```sh
./CPP_Labs --utest
```
