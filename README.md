# C++ Thread Pool

A lightweight, header-only thread pool library written in **C++20**, designed for concurrent task execution using a fixed number of worker threads.

This project was developed to explore modern C++ concurrency, RAII, asynchronous programming, thread synchronization, and automated testing.

## Features

- Fixed-size worker thread pool
- Thread-safe task queue using `std::mutex`
- Efficient worker synchronization with `std::condition_variable`
- Asynchronous task submission via `Submit()`
- Task results through `std::future` and `std::packaged_task`
- Automatic exception propagation through futures
- Graceful shutdown with completion of queued tasks
- Concurrent task submission from multiple producer threads
- Header-only CMake library

## Requirements

- C++20-compatible compiler (GCC or Clang)
- CMake 3.16+
- Linux or another environment with C++ threading support

## Project Structure

```text
cpp-thread-pool/
├── .github/
│   └── workflows/
│       └── ci.yml
├── include/
│   └── ThreadPool.hpp
├── src/
│   └── main.cpp
├── tests/
│   └── ThreadPoolTests.cpp
├── CMakeLists.txt
├── .gitignore
└── README.md
```

## Usage

```cpp
#include "ThreadPool.hpp"

#include <iostream>

int main()
{
    ThreadPool pool(3);

    auto future1 = pool.Submit([] {
        return 42;
    });

    auto future2 = pool.Submit([] {
        return 10 + 20;
    });

    std::cout << future1.get() << '\n';
    std::cout << future2.get() << '\n';
}
```

Worker threads execute submitted tasks concurrently. `Submit()` returns a future that can be used to retrieve the task result or rethrow an exception.

## Build

Configure the project:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
```

Build the example application:

```bash
cmake --build build --target ThreadPoolApp
```

Run:

```bash
./build/ThreadPoolApp
```

## Testing

The project uses **GoogleTest** and **CTest** for automated testing.

Tests cover:

- Rejection of invalid worker counts
- Task result retrieval through futures
- Exception propagation
- Completion of queued tasks during destruction
- Concurrent submissions from multiple producer threads
- Worker survival after task exceptions

Build the tests:

```bash
cmake --build build --target ThreadPoolTests
```

Run all tests:

```bash
ctest --test-dir build --output-on-failure
```

## ThreadSanitizer

The project supports optional ThreadSanitizer instrumentation for detecting data races.

Configure a separate build:

```bash
cmake -S . -B build-tsan \
    -DCMAKE_BUILD_TYPE=Debug \
    -DTHREADPOOL_ENABLE_TSAN=ON
```

Build and run the tests:

```bash
cmake --build build-tsan --target ThreadPoolTests

ctest --test-dir build-tsan --output-on-failure
```

ThreadSanitizer helps detect unsafe concurrent memory accesses that ordinary unit tests may not reveal.

## Continuous Integration

The project uses **GitHub Actions** to automatically build and test the library on Ubuntu.

The CI pipeline runs on every push and pull request and includes:

- GCC with C++20
- Clang with C++20
- GoogleTest and CTest execution

Both compiler configurations are tested independently.

## Implementation Details

The thread pool follows the producer-consumer pattern.

1. Producers submit callable tasks to a synchronized queue.
2. Worker threads wait on a condition variable.
3. Available workers retrieve and execute tasks.
4. Task results and exceptions are stored in shared states accessible through `std::future`.
5. During destruction, the pool completes accepted tasks and joins its workers.

## Limitations

This is an educational implementation rather than a production-ready thread-pool library.

Current limitations include:

- Fixed worker count
- No task priorities
- No task cancellation
- No dynamic worker resizing

Users must ensure the thread pool remains alive while other threads are submitting tasks.

## Technologies

C++20 · CMake · GCC · Clang · GoogleTest · CTest · ThreadSanitizer · AddressSanitizer · GitHub Actions