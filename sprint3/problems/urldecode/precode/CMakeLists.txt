cmake_minimum_required(VERSION 3.11)

project(urldecode CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

add_executable(urldecode
    src/main.cpp
    src/urldecode.h
    src/urldecode.cpp
)

add_executable(tests
    tests/tests.cpp
    src/urldecode.h
    src/urldecode.cpp
)
target_link_libraries(tests PRIVATE CONAN_PKG::boost)
