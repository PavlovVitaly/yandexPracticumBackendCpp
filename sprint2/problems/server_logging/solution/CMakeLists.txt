cmake_minimum_required(VERSION 3.11)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(Boost 1.80.0 REQUIRED)
find_package(Boost 1.80.0 COMPONENTS log log_setup)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(game_server
	src/main.cpp
	src/http_server.cpp
	src/http_server.h
	src/sdk.h
	src/model.h
	src/model.cpp
	src/tagged.h
	src/request_handler.cpp
	src/request_handler.h
	src/json/json_loader.h
	src/json/json_loader.cpp
	src/json/json_converter.cpp
	src/json/json_converter.h
	src/response_storage/api_v1_responce_storage.cpp
	src/response_storage/api_v1_responce_storage.h
	src/response_storage/static_file_response_storage.cpp
	src/response_storage/static_file_response_storage.h
	src/utils/filesystem_utils.cpp
	src/utils/filesystem_utils.h
	src/boost_json.cpp
	src/logging/logging_data_storage.cpp
	src/logging/logging_data_storage.h
	src/logging/logger.cpp
	src/logging/logger.h
)
target_include_directories(game_server PRIVATE
	src
	src/json
	src/response_storage
	src/utils
	src/logging
)
target_link_libraries(game_server PRIVATE Threads::Threads jsoncpp Boost::log Boost::log_setup)

# Boost.Beast будет использовать std::string_view вместо boost::string_view
add_compile_definitions(BOOST_BEAST_USE_STD_STRING_VIEW)
