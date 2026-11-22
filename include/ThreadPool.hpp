#pragma once

#include <cassert>
#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <future>
#include <chrono>
#include <queue>
#include <condition_variable>
#include <vector>
#include <stdexcept>
#include <utility>
#include <memory>
#include <type_traits>

class ThreadPool {
public:
    explicit ThreadPool(size_t threadCount) {

        if (threadCount == 0) {
            throw std::invalid_argument("threadCount must be greater than 0");
        }

        workers_.reserve(threadCount);
        for (size_t i = 0; i < threadCount; ++i) {
            try {
                workers_.emplace_back(&ThreadPool::Worker, this);
            }
            catch (...) {
                {
                    std::lock_guard<std::mutex> lock(m);
                    finished = true;
                }

                cv.notify_all();

                for (auto& worker : workers_) {
                    worker.join();
                }

                throw;
            }
        }
    }

    template<typename F>
    auto Submit(F&& func) {
        using Result = std::invoke_result_t<F&>;

        auto task = std::make_shared<std::packaged_task<Result()>>(
            std::forward<F>(func)
        );

        auto future = task->get_future();

        {
            std::lock_guard<std::mutex> lock(m);
            if (!finished) {
                std::function<void()> wrapper = [task] {
                    (*task)();
                };
                tasks.push(std::move(wrapper));
            }
            else {
                throw std::runtime_error("ThreadPool is stopped");
            }
        }
        cv.notify_one();

        return future;
    }

    ~ThreadPool() {
        {
            std::lock_guard<std::mutex> lock(m);
            finished = true;
        }

        cv.notify_all();

        for (auto& worker : workers_) {
            worker.join();
        }
    }

private:
    void Worker() {
        while (true) {
            std::unique_lock<std::mutex> lock(m);

            cv.wait(lock, [this] {
                return !tasks.empty() || finished;
            });

            if (tasks.empty()) {
                return;
            }

            std::function<void()> task = std::move(tasks.front());
            tasks.pop();

            lock.unlock();

            task();
        }
    }

    std::vector<std::thread> workers_;

    std::queue<std::function<void()>> tasks;


    std::mutex m;
    std::condition_variable cv;

    bool finished = false;
};