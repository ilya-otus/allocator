#include "lib.h"
#include <iostream>
#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>
#include "custom_container.h"
#include "reserve_allocator.h"

const size_t N = 10;
template <typename T>
using ReserveNAllocator = ReserveAllocator<T, N>;
int main(int , char **)
{
    auto defaultContainerDefaultAllocator = std::map<int, int>();
    for (size_t i = 0; i < N; ++i) {
        if (i == 0) {
            defaultContainerDefaultAllocator[i] = 1;
        } else {
            defaultContainerDefaultAllocator[i] = defaultContainerDefaultAllocator[i - 1] * i;
        }
    }

    auto defaultContainerCustomAllocator = std::map<int, int, std::less<int>, ReserveNAllocator<std::pair<const int, int> > >();
    for (size_t i = 0; i < N; ++i) {
        if (i == 0) {
            defaultContainerCustomAllocator[i] = 1;
        } else {
            defaultContainerCustomAllocator[i] = defaultContainerCustomAllocator[i - 1] * i;
        }
        std::cout << i << " " << defaultContainerCustomAllocator[i] << std::endl;
    }

    auto customContainerDefaultAllocator = CustomContainer<int>();
    for (size_t i = 0; i < 10; ++i) {
        customContainerDefaultAllocator.emplace_back(i);
    }

    auto customContainerCustomAllocator = CustomContainer<int, ReserveNAllocator<int>>();
    for (size_t i = 0; i < 10; ++i) {
        customContainerCustomAllocator.emplace_back(i);
        std::cout << customContainerCustomAllocator[i] << std::endl;
    }
    return 0;
}
