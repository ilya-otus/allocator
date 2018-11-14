#include "lib.h"
#include <iostream>
#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>
#include "custom_container.h"
#include "reserve_allocator.h"

int main(int , char **)
{
    auto a = CustomContainer<int, ReserveAllocator<int, 10>>();
    for (size_t i = 0; i < 10; ++i) {
        a.emplace_back(i);
    }
    return 0;
}
