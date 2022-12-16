cmake_minimum_required(VERSION 3.11)

project(bookypedia CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_library(libbookypedia STATIC
	src/menu.cpp
	src/menu.h
	src/view.cpp
	src/view.h
	src/use_cases.h
	src/use_cases_impl.cpp
	src/use_cases_impl.h
	src/author.cpp
	src/author.h
	src/author_fwd.h
	src/tagged.h
	src/tagged_uuid.cpp
	src/tagged_uuid.h
	src/postgres.cpp
	src/postgres.h
)
target_link_libraries(libbookypedia PUBLIC CONAN_PKG::boost Threads::Threads CONAN_PKG::libpq CONAN_PKG::libpqxx)

add_executable(bookypedia
	src/bookypedia.cpp
	src/bookypedia.h
	src/main.cpp
)
target_link_libraries(bookypedia PRIVATE CONAN_PKG::boost libbookypedia)

add_executable(tests
	tests/use_case_tests.cpp
	tests/tagged_uuid_tests.cpp
)
target_link_libraries(tests PRIVATE CONAN_PKG::catch2 CONAN_PKG::gtest libbookypedia)
