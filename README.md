# C++ Thread Pool

A lightweight, header-only thread pool written in **C++20**. It uses a fixed set of worker threads to execute submitted tasks concurrently and returns `std::future` objects for retrieving results and exceptions.

This is a learning project focused on modern C++, concurrency, RAII, and CMake.

## Features

- **Fixed-size worker pool** using `std::thread`
- **Thread-safe task queue** using `std::mutex` and `std::condition_variable`
- **Asynchronous task submission** with `Submit()`
- **Result retrieval** through `std::future`
- **Exception propagation** from tasks through their futures
- **Graceful shutdown** that finishes queued work and joins worker threads
- **Header-only library** usable through a CMake `INTERFACE` target

## Requirements

- A compiler supporting **C++20**
- **CMake 3.16** or newer
- Thread support (`Threads::Threads` in CMake)

Development has primarily taken place on Ubuntu under WSL2 using GCC and CLion. Other toolchains have not yet been verified.

## Project structure

```text
threadpool/
├── CMakeLists.txt
├── include/
│   └── ThreadPool.hpp
├── src/
│   └── main.cpp
└── tests/               # Unit tests in progress
```

## Build and run

From the project root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/ThreadPoolApp
```

The CMake project defines `ThreadPoolLib` as a header-only `INTERFACE` library and `ThreadPoolApp` as an example executable.

**Note:** The current development CMake configuration may enable AddressSanitizer for `ThreadPoolApp`. If you are building with a toolchain that does not support `-fsanitize=address`, disable those development-only flags before building.

## Usage

Include the header and create a pool with the desired number of worker threads:

```cpp
#include "ThreadPool.hpp"

#include <iostream>
#include <stdexcept>

int main()
{
    ThreadPool pool(3);

    auto answer = pool.Submit([] {
        return 42;
    });

    auto failure = pool.Submit([]() -> int {
        throw std::runtime_error("Task failed");
    });

    std::cout << "Result: " << answer.get() << '\n';

    try {
        failure.get();
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}
```

`Submit()` accepts a callable that takes no arguments. Use a lambda to capture any inputs you need. It returns a future for the task's result; exceptions thrown by the task are rethrown when the future's `get()` is called.

## How it works

1. The constructor starts a fixed number of worker threads.
2. `Submit()` wraps each callable in a `std::packaged_task` and places it in a synchronized queue.
3. Available workers wait on a condition variable, retrieve tasks, and execute them outside the queue's critical section.
4. During destruction, the pool stops accepting work, completes remaining queued tasks, and joins its worker threads.

## Testing and diagnostics

The project has been used to practice concurrency debugging with **ThreadSanitizer** and memory diagnostics with **AddressSanitizer**. Automated unit tests with **GoogleTest** and **CTest** are being added.

## Scope and limitations

This is an **educational implementation**, not a production-ready thread-pool library. It currently has a fixed number of worker threads and does not provide task priorities, cancellation, or dynamic resizing. Callers must ensure the pool remains alive while tasks are being submitted and that referenced objects outlive any tasks that access them.

## Planned improvements

- GoogleTest unit tests and CTest integration
- CMake configurations for Debug, Release, ASan, and TSan
- CI builds and test execution with GitHub Actions
- Additional stress tests and documentation