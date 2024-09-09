# Adaptive Simpson's Rule (ASI) Integration

This project implements the Adaptive Simpson's Rule (ASI) for numerical integration in C++. It includes a class `ASI` that can integrate a given function over a specified interval with a given tolerance. The project also includes unit tests to verify the functionality of the `ASI` class.

## Features

- Adaptive Simpson's Rule for numerical integration
- Customizable tolerance for integration
- Unit tests using the `utest` framework

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

The main program integrates the function ( f(x) = x + \cos(x^5) ) over the interval ([0, \pi]) with different tolerances and prints the results.  

## Testing

Unit tests are included in the project to verify the functionality of the ASI class. To run the tests, execute the following command from the build directory:

```sh
./CPP_Labs --utest
```
