cmake_minimum_required(VERSION 3.11)

project(postgres CXX)
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

add_executable(db_example
    src/main.cpp
    src/model.cpp
    src/boost_json.cpp
    src/db_functions.cpp
    src/middleware.cpp
)
target_include_directories(db_example PUBLIC 
	src
)
target_link_libraries(db_example PRIVATE CONAN_PKG::libpqxx CONAN_PKG::boost) 
