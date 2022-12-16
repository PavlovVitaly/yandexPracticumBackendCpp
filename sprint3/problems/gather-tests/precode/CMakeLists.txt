cmake_minimum_required(VERSION 3.11)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_library(collision_detection_lib STATIC
	src/collision_detector.h
	src/collision_detector.cpp
)

target_link_libraries(collision_detection_lib PUBLIC CONAN_PKG::boost Threads::Threads)

add_executable(collision_detection_tests
	tests/collision-detector-tests.cpp
)

target_link_libraries(collision_detection_tests CONAN_PKG::catch2 collision_detection_lib)
