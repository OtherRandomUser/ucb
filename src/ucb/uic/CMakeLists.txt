target_sources(uic
    PRIVATE
        main.cpp
)

target_include_directories(uic
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
    PRIVATE
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(uic
    ucb-frontend
    ${Boost_LIBRARIES}
)
