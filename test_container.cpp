#define BOOST_TEST_MODULE callocator_test_module
#include <map>
#include "reserve_allocator.h"
#include "custom_container.h"
#include <boost/test/unit_test.hpp>
const size_t N = 10;
template <typename T>
using ReserveNAllocator = ReserveAllocator<T, N>;
BOOST_AUTO_TEST_SUITE(container_test_suite)
BOOST_AUTO_TEST_CASE(default_allocator_default_container_test) {
    auto test = std::map<int, int>();
    for (size_t i = 0; i < N; ++i) {
        test[i] = i;
    }
    BOOST_CHECK_EQUAL(test.size(), N);
}

BOOST_AUTO_TEST_CASE(custom_allocator_default_container_test) {
    auto test = std::map<int, int, std::less<int>, ReserveAllocator<std::pair<const int, int>, N> >();
    for (size_t i = 0; i < N; ++i) {
        test[i] = i;
    }
    BOOST_CHECK_EQUAL(test.size(), N);
}

BOOST_AUTO_TEST_CASE(default_allocator_custom_container_test) {
    auto test = CustomContainer<int>();
    for (size_t i = 0; i < N; ++i) {
        test.emplace_back(i);
    }
    BOOST_CHECK_EQUAL(test.size(), N);
}

BOOST_AUTO_TEST_CASE(custom_allocator_custom_container_test) {
    auto test = CustomContainer<int, ReserveAllocator<int, N> >();
    for (size_t i = 0; i < N; ++i) {
        test.emplace_back(i);
    }
    BOOST_CHECK_EQUAL(test.size(), N);
}

BOOST_AUTO_TEST_CASE(custom_allocator_custom_container_assignment_test) {
    auto customContainerCustomAllocator = CustomContainer<int, ReserveNAllocator<int>>();
    for (size_t i = 0; i < N; ++i) {
        customContainerCustomAllocator.emplace_back(i);
    }
    CustomContainer<int, ReserveNAllocator<int>> testCopy;
    testCopy = customContainerCustomAllocator;
    BOOST_CHECK(customContainerCustomAllocator == testCopy);
}

BOOST_AUTO_TEST_CASE(custom_allocator_custom_container_copy_test) {
    auto customContainerCustomAllocator = CustomContainer<int, ReserveNAllocator<int>>();
    for (size_t i = 0; i < N; ++i) {
        customContainerCustomAllocator.emplace_back(i);
    }
    CustomContainer<int, ReserveNAllocator<int>> testCopy(customContainerCustomAllocator);
    BOOST_CHECK(customContainerCustomAllocator == testCopy);
}

BOOST_AUTO_TEST_CASE(custom_allocator_custom_container_move_test) {
    auto customContainerCustomAllocator = CustomContainer<int, ReserveNAllocator<int>>();
    for (size_t i = 0; i < N; ++i) {
        customContainerCustomAllocator.emplace_back(i);
    }
    CustomContainer<int, ReserveNAllocator<int>> testCopy(customContainerCustomAllocator);
    CustomContainer<int, ReserveNAllocator<int>> testMove(testCopy);
    BOOST_CHECK(customContainerCustomAllocator == testMove);
}

BOOST_AUTO_TEST_SUITE_END()
