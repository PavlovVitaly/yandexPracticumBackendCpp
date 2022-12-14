cmake_minimum_required(VERSION 3.11)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(Boost 1.78.0 REQUIRED)
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
)
target_include_directories(game_server PRIVATE
	src
	src/json
	src/response_storage
	src/utils
)
target_link_libraries(game_server PRIVATE Threads::Threads jsoncpp)
# Boost.Beast будет использовать std::string_view вместо boost::string_view
add_compile_definitions(BOOST_BEAST_USE_STD_STRING_VIEW)