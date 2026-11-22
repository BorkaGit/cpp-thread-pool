#include <gtest/gtest.h>
#include "ThreadPool.hpp"
#include <atomic>
#include <thread>
#include <vector>

TEST(ThreadPoolTest, RejectsZeroWorkers)
{
    EXPECT_THROW(ThreadPool(0), std::invalid_argument);
}

TEST(ThreadPoolTest, ReturnTaskResult)
{
    ThreadPool pool(2);
    auto future = pool.Submit([] { return 42; });

    EXPECT_EQ(42, future.get());
}

TEST(ThreadPoolTest, PropagatesExceptions)
{
    ThreadPool pool(2);
    auto future = pool.Submit([] { throw std::runtime_error(""); });

    EXPECT_THROW(future.get(), std::runtime_error);
}

TEST(ThreadPoolTest, CompleteAllTasksOnDestruction)
{
    std::atomic<int> counter{0};

    {
        ThreadPool pool(3);

        for (int i = 0; i < 100; ++i) {
            pool.Submit([&counter]{ ++counter; });
        }
    }

    EXPECT_EQ(100, counter.load());
}

TEST(ThreadPoolTest, ConcurrentSubmissions)
{
    std::atomic<int> counter{0};

    {
        ThreadPool pool(3);
        std::vector<std::thread> producers;

        for (int i = 0; i < 4; ++i) {
            producers.emplace_back([&pool, &counter] {
                for (int j = 0; j < 250; ++j) {
                    pool.Submit([&counter]{ ++counter; });
                }
            });
        }

        for (auto& producer : producers) {
            producer.join();
        }
    }

    EXPECT_EQ(1000, counter.load());
}

TEST(ThreadPoolTest, WorkerSurvivesTaskException)
{
    ThreadPool pool(1);

    auto future1 = pool.Submit([] { throw std::runtime_error(""); });
    auto future2 = pool.Submit([] { return 42; });

    EXPECT_THROW(future1.get(), std::runtime_error);
    EXPECT_EQ(42, future2.get());
}