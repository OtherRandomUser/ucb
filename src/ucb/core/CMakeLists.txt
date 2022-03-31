target_sources(ucb-core
    PUBLIC
        pass-manager.hpp
        target-machine.hpp
        ir/basic-block.hpp
        ir/compile-unit.hpp
        ir/instruction.hpp
        ir/procedure.hpp
        ir/type.hpp
        ir/value.hpp
    PRIVATE
        pass-manager.cpp
        target-machine.cpp
        ir/basic-block.cpp
        ir/compile-unit.cpp
        ir/instruction.cpp
        ir/procedure.cpp
)

target_include_directories(ucb-core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(ucb-core
    fmt::fmt
)
