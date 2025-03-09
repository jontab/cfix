cmake_minimum_required(VERSION 3.0)
project(cfix LANGUAGES C)

#
# cfix
#

file(GLOB_RECURSE CFIX_SOURCES src/*.c src/**/*.c)
file(GLOB_RECURSE CFIX_HEADERS src/*.h src/**/*.h)

add_library(cfix SHARED ${CFIX_SOURCES} ${CFIX_HEADERS})

target_include_directories(cfix PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

#
# cfix-tests
#

file(GLOB_RECURSE TEST_SOURCES tests/*.c tests/**/*.c)
file(GLOB_RECURSE TEST_HEADERS tests/*.h tests/**/*.h)

list(APPEND TEST_SOURCES deps/munit/munit.c)
list(APPEND TEST_HEADERS deps/munit/munit.h)

add_executable(cfix-tests ${TEST_SOURCES} ${TEST_HEADERS})

target_link_libraries(cfix-tests cfix)

target_include_directories(cfix-tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/munit
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
