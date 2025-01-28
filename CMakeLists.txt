cmake_minimum_required(VERSION 3.5)
project(cfix)

set(CMAKE_C_FLAGS_DEBUG "-Wall -Werror -pedantic -O0 -g")
set(CMAKE_C_FLAGS_RELEASE "-Wall -Werror -pedantic -O3")

set(LIB_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tcp/client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tcp/server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tcp/transport.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/client.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/server.c)

set(LIB_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cfix/client.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cfix/server.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cfix/transport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tcp/client.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tcp/server.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tcp/transport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/memory.h)

set(TESTS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/unit_tcp_server.c
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/munit/munit.c)

set(TESTS_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/munit/munit.h)

##
# cfix-tests
##

add_executable(cfix-tests
    ${LIB_SOURCES}
    ${LIB_HEADERS}
    ${TESTS_SOURCES}
    ${TESTS_HEADERS})

target_include_directories(cfix-tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/munit)
