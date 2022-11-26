cmake_minimum_required(VERSION 3.11)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(Boost 1.78.0 REQUIRED)
find_package(Boost 1.78.0 COMPONENTS log log_setup)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(game_server
	src/main.cpp
	src/http_server.cpp
	src/model/model.cpp
	src/request_handler.cpp
	src/json/json_loader.cpp
	src/json/json_converter.cpp
	src/request_handlers/request_handlers_utility.cpp
	src/utils/filesystem_utils.cpp
	src/boost_json.cpp
	src/logging/logging_data_storage.cpp
	src/logging/logger.cpp
)
target_include_directories(game_server PRIVATE
	src
	src/json
	src/request_handlers
	src/utils
	src/logging
	src/model
)
target_link_libraries(game_server PRIVATE Threads::Threads Boost::log Boost::log_setup)

# Boost.Beast будет использовать std::string_view вместо boost::string_view
add_compile_definitions(BOOST_BEAST_USE_STD_STRING_VIEW)
