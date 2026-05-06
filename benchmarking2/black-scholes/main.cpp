#include "black–scholes.hpp"
#include <chrono>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>

int main()
{
    const auto N = 1000;
    std::vector<double> results(N);

    auto num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0)
        num_threads = 1;

    std::cout << "Running on " << num_threads << " threads" << std::endl;

    auto worker = [&](int start, int end)
    {
        for (int i = start; i < end; ++i)
        {
            // правило - меняем страйк в зависимости от опциона
            double strike = 80.0 + (i * 0.04);
            NumMethods::BS bs{100, strike, 0.1, 0.05, 1};
            results[i] = bs.get_price();
        }
    };

    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    int chunk_size = N / num_threads;

    for (size_t i = 0; i < num_threads; ++i)
    {
        auto start = i * chunk_size;
        auto end   = (i == num_threads - 1) ? N : (start + chunk_size);

        threads.emplace_back(worker, start, end);
    }

    for (auto& t : threads)
    {
        t.join();
    }

    auto end_time                                      = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end_time - start_time;

    std::cout << "First option price: " << results[0] << std::endl;
    std::cout << "Last option price: " << results[N - 1] << std::endl;
    std::cout << "Wall time: " << duration.count() << " ms" << std::endl;

    return EXIT_SUCCESS;
}