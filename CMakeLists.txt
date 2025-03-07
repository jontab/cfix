cmake_minimum_required(VERSION 3.6)
project(cfix C)

add_executable(cfix
    deps/munit/munit.c
    deps/munit/munit.h
    include/cfix/engine.h
    include/cfix/settings.h
    src/settings.c
    tests/main.c
    tests/unit_settings.c)

add_subdirectory(deps/libyaml)

target_include_directories(cfix PRIVATE
    deps/libyaml/include
    deps/munit
    include)

target_link_libraries(cfix PRIVATE yaml)

file(COPY data DESTINATION data)

target_compile_options(cfix PUBLIC -O0 -g)
