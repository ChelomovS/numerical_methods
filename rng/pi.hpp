#pragma once

#include "third_party/pcg_random.hpp"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

namespace NumMethods
{

class PiRealization final
{
private:
    size_t number_of_points;
    size_t threads_number;
    std::atomic<size_t> total_inside_circle = 0;

    void work_for_one_thread(size_t points_for_process, uint64_t global_seed, uint64_t skip_count)
    {
        pcg32 gen(global_seed);

        gen.advance(skip_count);

        std::uniform_real_distribution<double> dist(0.0, 1.0);
        size_t local_inside_circle = 0;

        for (size_t number_of_point = 0; number_of_point < points_for_process; ++number_of_point)
        {
            auto x = dist(gen);
            auto y = dist(gen);
            if ((x * x + y * y) <= 1.0)
                ++local_inside_circle;
        }
        total_inside_circle += local_inside_circle;
    }

public:
    PiRealization(size_t number_of_points, size_t threads_number = std::thread::hardware_concurrency())
        : number_of_points{number_of_points}, threads_number{std::max<size_t>(1, threads_number)}
    {
    }

    double operator()()
    {
        std::random_device rd;
        uint64_t global_seed = (static_cast<uint64_t>(rd()) << 32) | rd();

        std::vector<std::thread> threads;
        size_t reminder = number_of_points % threads_number;

        uint64_t current_skip = 0;

        for (size_t thread_counter = 0; thread_counter < threads_number; ++thread_counter)
        {
            size_t points_for_this_thread = number_of_points / threads_number + (thread_counter < reminder ? 1 : 0);

            threads.emplace_back(&PiRealization::work_for_one_thread, this, points_for_this_thread, global_seed, current_skip);

            current_skip += static_cast<uint64_t>(points_for_this_thread) * 2;
        }

        std::for_each(threads.begin(), threads.end(), [](std::thread& thread) { thread.join(); });

        const double PI_REF  = 3.14159265358979323846;
        double calculated_pi = 4.0 * static_cast<double>(total_inside_circle) / static_cast<double>(number_of_points);
        double abs_error     = std::abs(calculated_pi - PI_REF);

        double std_dev          = std::sqrt(PI_REF * (4.0 - PI_REF) / static_cast<double>(number_of_points));
        double confidence_limit = 3.0 * std_dev; // три сигмы

        std::cout << "[Monte carlo report | Library: PCG]\n";
        std::cout << std::fixed << std::setprecision(15);
        std::cout << "Calculated Pi: " << calculated_pi << "\n";
        std::cout << "Reference Pi:  " << PI_REF << "\n";
        std::cout << "Abs Error:     " << std::scientific << abs_error << std::fixed << "\n";
        std::cout << "3-Sigma Limit: " << confidence_limit << "\n";

        if (abs_error <= confidence_limit)
            std::cout << "Result is statistically valid (within 3 sigma).\n";
        else
            std::cout << "Result is outside of 3 sigma.\n";

        return calculated_pi;
    }
};

} // namespace NumMethods
