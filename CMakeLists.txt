cmake_minimum_required(VERSION 3.16)

project(ThreadPool)

option(THREADPOOL_ENABLE_TSAN "Enable ThreadSanitizer" OFF)

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

#GoogleTest
include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.18.0
        GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(googletest)

#Testing
enable_testing()

add_executable(ThreadPoolTests
    tests/ThreadPoolTests.cpp
)

target_link_libraries(ThreadPoolTests PRIVATE
        ThreadPoolLib
        GTest::gtest_main
)

include (GoogleTest)
gtest_discover_tests(ThreadPoolTests)

if (THREADPOOL_ENABLE_TSAN)
    target_compile_options(ThreadPoolTests PRIVATE
        -fsanitize=thread
        -fno-omit-frame-pointer
        -g
        -01
    )

    target_link_options(ThreadPoolTests PRIVATE
    -fsanitize=thread
    )
endif()