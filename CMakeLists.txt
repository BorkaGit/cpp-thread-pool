cmake_minimum_required(VERSION 3.16)

project(ThreadPool)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_library(ThreadPoolLib INTERFACE)

target_include_directories(ThreadPoolLib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(ThreadPoolLib INTERFACE Threads::Threads)

add_executable(ThreadPoolApp src/main.cpp)

target_link_libraries(ThreadPoolApp PRIVATE ThreadPoolLib)

target_compile_options(ThreadPoolApp PRIVATE
        -fsanitize=address
        -fno-omit-frame-pointer
        -g
        -O0
)

target_link_options(ThreadPoolApp PRIVATE
        -fsanitize=address
)