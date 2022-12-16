cmake_minimum_required(VERSION 3.11)

project(htmldecode CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

add_executable(htmldecode
    src/main.cpp
    src/htmldecode.h
    src/htmldecode.cpp
)

add_executable(tests
    tests/tests.cpp
    src/htmldecode.h
    src/htmldecode.cpp
)
target_link_libraries(tests PRIVATE CONAN_PKG::catch2)
