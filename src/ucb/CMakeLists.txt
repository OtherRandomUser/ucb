configure_file(
    ${CMAKE_SOURCE_DIR}/src/ucb/core/config.hpp.in
    ${CMAKE_SOURCE_DIR}/src/ucb/core/config.hpp)

add_subdirectory(core)
add_subdirectory(frontend)
add_subdirectory(uic)
