target_sources(ucb-core
    PUBLIC
        ir/basic-block.hpp
        ir/compile-unit.hpp
        ir/instruction.hpp
        ir/procedure.hpp
        ir/type.hpp
        ir/value.hpp
    PRIVATE
)

target_include_directories(ucb-core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(ucb-core
    fmt::fmt
)
