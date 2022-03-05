target_sources(ucb-frontend
    PUBLIC
        lexer.hpp
        parser.hpp
        token.hpp
    PRIVATE
        lexer.cpp
        parser.cpp
)

target_include_directories(ucb-frontend
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(ucb-frontend
    fmt::fmt
)
