cmake_minimum_required(VERSION 3.11)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wall")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Boost 1.78.0 REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_library(collision_detection_lib STATIC
	src/model/physics/collision_detector.h
	src/model/physics/collision_detector.cpp
	src/model/physics/geom.h
)
target_link_libraries(collision_detection_lib PUBLIC CONAN_PKG::boost Threads::Threads)

add_library(game_model_lib STATIC
	src/model/map.cpp
	src/model/game.cpp
	src/model/dog.cpp
	src/model/roadmap.cpp
	src/model/support_types.cpp
	src/model/loot_generator.cpp
	src/model/lost_object.cpp
	src/model/item_dog_provider.cpp
	src/utils/request_handlers_utils.cpp
	src/utils/filesystem_utils.cpp
	src/utils/random_generators.cpp
)
target_include_directories(game_model_lib PUBLIC 
	${ZLIB_INCLUDES}
	src/model
	src/model/physics
	src/utils
)
target_link_libraries(game_model_lib PUBLIC ${BOOST_LIB} ${ZLIB_LIB} collision_detection_lib)

add_executable(game_server
	src/main.cpp
	src/json/json_loader.cpp
	src/json/json_converter.cpp
	src/json/json_model_converter.cpp
	src/request_handlers/request_handler.cpp
	src/boost_json.cpp
	src/logging/logging_data_storage.cpp
	src/logging/logger.cpp
	src/authentication/player_tokens.cpp
	src/app/application.cpp
	src/app/game_session.cpp
	src/app/player.cpp
	src/server/http_server.cpp
	src/program_options/program_options.cpp
	src/time_management/ticker.cpp
	src/error_handling/error_report.cpp
)
target_include_directories(game_server PRIVATE
	src
	src/json
	src/request_handlers
	src/logging
	src/authentication
	src/app
	src/server
	src/program_options
	src/time_management
	src/error_handling
)
target_link_libraries(game_server PRIVATE Threads::Threads CONAN_PKG::boost game_model_lib)

include(CTest)
include(${CONAN_BUILD_DIRS_CATCH2}/Catch.cmake)

add_executable(game_server_tests
	tests/loot_generator_tests.cpp
)
target_link_libraries(game_server_tests PRIVATE CONAN_PKG::catch2 game_model_lib)
catch_discover_tests(game_server_tests)

add_executable(collision_detection_tests
	tests/collision-detector-tests.cpp
)
target_link_libraries(collision_detection_tests CONAN_PKG::catch2 collision_detection_lib)
catch_discover_tests(collision_detection_tests)
# Boost.Beast будет использовать std::string_view вместо boost::string_view
add_compile_definitions(BOOST_BEAST_USE_STD_STRING_VIEW)
add_compile_definitions(BOOST_THREAD_PROVIDES_FUTURE)
