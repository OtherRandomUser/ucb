target_sources(uic
    PRIVATE
        main.cpp
)

target_include_directories(uic
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(uic
    ucb-frontend
)
