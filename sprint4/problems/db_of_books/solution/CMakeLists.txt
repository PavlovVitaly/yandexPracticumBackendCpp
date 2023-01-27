cmake_minimum_required(VERSION 3.11)

project(book_manager CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wall")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

find_package(Boost 1.78.0 REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(book_manager
    src/main.cpp
    src/model.cpp
    src/boost_json.cpp
    src/db_functions.cpp
    src/middleware.cpp
)
target_include_directories(book_manager PUBLIC 
	src
)
target_link_libraries(book_manager PRIVATE CONAN_PKG::libpqxx CONAN_PKG::boost) 
