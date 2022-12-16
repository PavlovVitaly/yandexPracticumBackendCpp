cmake_minimum_required(VERSION 3.11)

project(tv CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

add_executable(app
	src/main.cpp
	src/tv.h
	src/menu.h
	src/controller.h
)

add_executable(catch_tv_tests
	tests/catch_tv_tests.cpp
	src/tv.h
	src/menu.h
	src/controller.h
)
target_link_libraries(catch_tv_tests PRIVATE CONAN_PKG::catch2)

add_executable(google_tv_tests
	tests/google_tv_tests.cpp
	src/tv.h
	src/menu.h
	src/controller.h
)
target_link_libraries(google_tv_tests PRIVATE CONAN_PKG::gtest)

add_executable(boost_tv_tests
	tests/boost_tv_tests.cpp
	src/tv.h
	src/menu.h
	src/controller.h
)
target_link_libraries(boost_tv_tests PRIVATE CONAN_PKG::boost)
