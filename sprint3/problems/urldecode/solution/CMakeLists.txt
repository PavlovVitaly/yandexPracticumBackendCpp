cmake_minimum_required(VERSION 3.11)

project(urldecode CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

find_package(Boost 1.78.0 REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()


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
target_link_libraries(tests PRIVATE CONAN_PKG::boost boost_unit_test_framework)
# Boost.Beast будет использовать std::string_view вместо boost::string_view
add_compile_definitions(BOOST_BEAST_USE_STD_STRING_VIEW)