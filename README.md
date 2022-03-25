# UCS Compiler Backend

## Usage Instructions

Right now the compiler only dumps the ir to stdout, to do that execute `./uic [input-file]`. Sample files are available on the `samples` folder.

## Compile Instructions

To compile, you'll need a c++ compiler (clang is recommended) and cmake, ninja is also recommended over make. To install everything on ubuntu, run `sudo apt install build-essential clang cmake ninja-build`.

### Installing the Dependencies

Currently, the compiler has four dependencies. Boost can be installed from the system package manager with `sudo apt install libboost-all-dev`. The remaining packages must be installed from source, to do that follow the instructions on their respective repositories:

* [fmt](https://github.com/fmtlib/fmt)
* [Catch2](https://github.com/catchorg/Catch2)
* [json](https://github.com/nlohmann/json)

### Compiling

After all the dependencies are installed, execute the following commands:
```
$ mkdir build && cd build
$ cmake .. -G Ninja -D CMAKE_BUILD_TYPE=Debug -D CMAKE_CXX_COMPILER=clang++ -D CMAKE_C_COMPILER=clang -D CMAKE_EXPORT_COMPILE_COMMANDS=On
