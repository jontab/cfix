cmake_minimum_required(VERSION 3.5)
project(cfix)

set(LIBCFIX_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cfix/message.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cfix/parser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cfix/ring.c)

set(LIBCFIX_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cfix/message.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cfix/parser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cfix/ring.h)

set(TESTS_SOURCES
    ${LIBCFIX_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/munit/munit.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/unit_message.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/unit_parser.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/unit_ring.c)

set(TESTS_HEADERS
    ${LIBCFIX_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/munit/munit.h)

################################################################################
# cfix-tests                                                                   #
################################################################################

add_executable(cfix-tests ${TESTS_SOURCES} ${TESTS_HEADERS})

target_include_directories(cfix-tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/munit)

target_compile_options(cfix-tests PUBLIC -O0 -g -Wall -Werror -pedantic -std=gnu11)

################################################################################
# cfix-benchmark                                                               #
################################################################################
