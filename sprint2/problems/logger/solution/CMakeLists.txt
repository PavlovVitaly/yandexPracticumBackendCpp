cmake_minimum_required(VERSION 3.9)
project(HelloLog CXX)
set(CMAKE_CXX_STANDARD 20)

# обратите внимание на аргумент TARGETS у команды conan_basic_setup
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_executable(hello_log main.cpp)

# используем "импортированную" цель CONAN_PKG::boost
target_include_directories(hello_log PRIVATE CONAN_PKG::boost)
target_link_libraries(hello_log CONAN_PKG::boost)
