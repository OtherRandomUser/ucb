target_sources(ucb-frontend
    PUBLIC
        lexer.hpp
        parser.hpp
        span-reader.hpp
    PRIVATE
        lexer.cpp
        span-reader.cpp
)

target_include_directories(ucb-frontend
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
)
