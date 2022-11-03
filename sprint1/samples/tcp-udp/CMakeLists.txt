cmake_minimum_required(VERSION 3.11)

# Проект называется TCP_UDP_Practice и написан на C++
project(TCP_UDP_Practice CXX)
# Исходый код будет компилироваться с поддержкой стандарта С++ 20
set(CMAKE_CXX_STANDARD 20)

# Подключаем сгенерированный скрипт conanbuildinfo.cmake, созданный Conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
# Выполняем макрос из conanbuildinfo.cmake, который настроит СMake на работу с библиотеками, установленными Conan
conan_basic_setup()

# Добавляем четыре проекта
add_executable(TCPServer tcp_server.cpp)
add_executable(TCPClient tcp_client.cpp)
add_executable(UDPServer udp_server.cpp)
add_executable(UDPClient udp_client.cpp)

# Просим компоновщик подключить библиотеку для поддержки потоков
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(TCPServer PRIVATE Threads::Threads)
target_link_libraries(TCPClient PRIVATE Threads::Threads)
target_link_libraries(UDPServer PRIVATE Threads::Threads)
target_link_libraries(UDPClient PRIVATE Threads::Threads)