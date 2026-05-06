#include "pi.hpp"
#include <iostream>

int main()
{
    size_t points_number;
    std::cin >> points_number;
    NumMethods::PiRealization pi(points_number);
    std::cout << pi() << std::endl;
}