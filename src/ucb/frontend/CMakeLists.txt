target_sources(ucb-frontend
    PUBLIC
        lexer.hpp
        parser.hpp
    PRIVATE
        lexer.cpp
)

target_include_directories(ucb-frontend
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
)
