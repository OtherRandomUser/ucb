cmake_minimum_required(VERSION 3.13S)
project(ucb VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)

find_package(fmt REQUIRED)
message(STATUS "Using {fmt} ${fmt_VERSION}")

find_package(nlohmann_json REQUIRED)
message(STATUS "Using nlohmann/json ${nlohman_json_VERSION}")

find_package(Catch2 REQUIRED)
message(STATUS "Using Catch2 ${Catch2_VERSION}")

find_package(Boost REQUIRED filesystem program_options)
message(STATUS "Using Boost ${Boost_VERSION}")

# add_library(ucb-core STATIC "")
add_library(ucb-frontend STATIC "")
add_executable(uic "")

add_subdirectory(src/ucb)
