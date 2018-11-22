#pragma once
#include <iostream>
#include <vector>
#include <map>

template<typename T, size_t N>
struct ReserveAllocator {
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;

    static constexpr size_t itemsCount = N;
    using buffer = std::vector<T*>;
    template<typename U, size_t M = itemsCount>
    struct rebind {
        using other = ReserveAllocator<U, M>;
    };

    ReserveAllocator()
    : mLastPos(0) {
        auto buf = std::malloc(N * sizeof(T));
        if (!buf) {
            throw std::bad_alloc();
        }
        mBuffer = reinterpret_cast<T *>(buf);
    }

    ReserveAllocator(const ReserveAllocator &other)
    : mBuffer(other.mBuffer),
      mLastPos(other.mLastPos) {
    }

    T *allocate(std::size_t n) {
        if (mLastPos + n > N)
            throw std::bad_alloc();
        auto p = &mBuffer[mLastPos];
        mLastPos += n;
        if (!p) {
            throw std::bad_alloc();
        }
        return reinterpret_cast<T *>(p);
    }

    void deallocate(T *p, std::size_t n) {
        (void)p;
        mLastPos -= n;
        if (mLastPos == 0) {
            std::free(mBuffer);
        }
    }

    template<typename U, typename ...Args>
    void construct(U *p, Args &&...args) {
        new(p) U(std::forward<Args>(args)...);
    }

    template<typename U>
    void destroy(U *p) {
        p->~U();
    }

private:
    T *mBuffer;
    size_t mLastPos;

};
