cmake_minimum_required(VERSION 3.11)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wall")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Boost 1.81.0 REQUIRED)
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
	src/middleware/utils/request_handlers_utils.cpp
	src/middleware/utils/filesystem_utils.cpp
	src/middleware/utils/random_generators.cpp
)
target_include_directories(game_model_lib PUBLIC 
	${ZLIB_INCLUDES}
	src/model
	src/model/physics
	src/middleware/utils
)
target_link_libraries(game_model_lib PUBLIC ${BOOST_LIB} ${ZLIB_LIB} collision_detection_lib)

add_executable(game_server
	src/main.cpp
	src/middleware/json/json_loader.cpp
	src/middleware/json/json_converter.cpp
	src/middleware/json/json_model_converter.cpp
	src/middleware/json/boost_json.cpp
	src/middleware/logging/logging_data_storage.cpp
	src/middleware/logging/logger.cpp
	src/middleware/program_options/program_options.cpp
	src/middleware/time_management/ticker.cpp
	src/middleware/error_handling/error_report.cpp
	src/middleware/game_data_store/lost_object_serialization.cpp
	src/middleware/game_data_store/dog_serialization.cpp
	src/middleware/game_data_store/player_serialization.cpp
	src/middleware/game_data_store/game_session_serialization.cpp
	src/web/request_handlers/request_handler.cpp
	src/web/server/http_server.cpp
	src/app/authentication/token_generator.cpp
	src/app/application.cpp
	src/app/game_session.cpp
	src/app/player.cpp
	src/database/postgres.cpp
	src/database/use_cases_impl.cpp
)
target_include_directories(game_server PRIVATE
	src
	src/app
	src/web
	src/middleware
	src/middleware/json
	src/web/request_handlers
	src/middleware/logging
	src/app/authentication
	src/web/server
	src/middleware/program_options
	src/middleware/time_management
	src/middleware/error_handling
	src/middleware/game_data_store
	src/database
)
target_link_libraries(game_server PRIVATE
	Threads::Threads
	CONAN_PKG::boost
	CONAN_PKG::libpq
	CONAN_PKG::libpqxx
	game_model_lib
)

#SET(GCC_COVERAGE_LINK_FLAGS    "-lboost_url")

include(CTest)
include(${CONAN_BUILD_DIRS_CATCH2}/Catch.cmake)

add_executable(game_server_tests
	tests/loot-generator-tests.cpp
)
target_link_libraries(game_server_tests PRIVATE CONAN_PKG::catch2 game_model_lib)
catch_discover_tests(game_server_tests)

add_executable(collision_detection_tests
	tests/collision-detector-tests.cpp
)
target_link_libraries(collision_detection_tests CONAN_PKG::catch2 collision_detection_lib)
catch_discover_tests(collision_detection_tests)

add_executable(state_serialization_tests
	tests/state-serialization-tests.cpp
)
target_include_directories(state_serialization_tests PRIVATE
	src
	src/middleware
	src/middleware/game_data_store
	src/model/physics
)
target_link_libraries(state_serialization_tests CONAN_PKG::catch2 collision_detection_lib)
catch_discover_tests(state_serialization_tests)

add_compile_definitions(BOOST_THREAD_PROVIDES_FUTURE)
