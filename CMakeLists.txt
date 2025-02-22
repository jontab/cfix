cmake_minimum_required(VERSION 3.5)
project(cfix)

set(CMAKE_C_FLAGS_DEBUG "-Wall -Werror -pedantic -O0 -g")
set(CMAKE_C_FLAGS_RELEASE "-Wall -Werror -pedantic -O3")

set(LIB_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fix40/tag.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tcp/acceptor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tcp/initiator.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tcp/transport.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/acceptor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/initiator.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/message.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/string.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/transport.c)

set(LIB_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cfix/acceptor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cfix/initiator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cfix/list.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cfix/memory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cfix/message.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cfix/session.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cfix/string.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cfix/transport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tcp/acceptor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tcp/close.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tcp/initiator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tcp/transport.h)

set(TESTS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/context.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/unit_message.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/unit_recv_chunker.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/unit_send_chunker.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/unit_tcp_acceptor.c
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/munit/munit.c)

set(TESTS_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/context.h
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
