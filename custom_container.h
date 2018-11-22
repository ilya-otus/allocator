#pragma once
#include <iterator>
#include <memory>

template <class T, typename A = std::allocator<T> >
struct CustomContainer {
    using allocator_type = A;
    using value_type = typename A::value_type;
    using reference = typename A::reference;
    using const_reference = typename A::const_reference;
    using pointer = typename A::pointer;
    using size_type = typename A::size_type;

    struct Field {
        Field *prev;
        Field *next;
        value_type value;
        bool operator==(const Field &other) const {
            return prev == other.prev &&
                next == other.next &&
                value == other.value;
        }
        bool operator!=(const Field &other) const {
            return prev != other.prev &&
                next != other.next &&
                value != other.value;
        }
    };
    using FieldAllocator = typename std::allocator_traits<A>::template rebind_alloc<Field>;

    class const_iterator;
    class iterator {
    public:
        friend const_iterator;
        using iterator_category = std::random_access_iterator_tag;
        iterator(Field *ptr) : mData(ptr) {}
        iterator(const iterator &other) : mData(other.mData) {}
        ~iterator() = default;
        iterator &operator=(const iterator &other) {
            mData = other.mData;
        }
        bool operator==(const iterator &other) const {
            return mData == other.mData;
        }
        bool operator!=(const iterator &other) const {
            return mData != other.mData;
        }
        iterator operator++() {
            mData = mData->next;
            return *this;
        }
        reference operator*() const {
            return mData->value;
        }
        pointer operator->() const {
            return &mData->value;
        }
    private:
        Field *mData;
    };

    class const_iterator {
    public:
        friend iterator;
        using reference = typename A::const_reference;
        using pointer = typename A::const_pointer;
        using iterator_category = std::random_access_iterator_tag;
        const_iterator(Field ptr) : mData(ptr) {}
        const_iterator(const const_iterator &other) : mData(other.mData) {}
        const_iterator(const iterator &other) : mData(other.mData) {}
        ~const_iterator() = default;
        iterator &operator=(const iterator &other) {
            mData = other.mData;
        }
        bool operator==(const iterator &other) const {
            return mData == other.mData;
        }
        bool operator!=(const iterator &other) const {
            return mData != other.mData;
        }
        bool operator!=(const_iterator &other) const {
            return mData != other.mData;
        }
        const_iterator operator++() {
            mData = mData->next;
            return *this;
        }
        reference operator*() const {
            return mData->value;
        }
        pointer operator->() const {
            return &mData->value;
        }
    private:
        Field *mData;
    };

    CustomContainer()
    : mBegin(nullptr),
      mLast(nullptr),
      mAllocator(FieldAllocator()),
      mSize(0) {
        mEnd.value = value_type();
        mEnd.next = nullptr;
    }

    CustomContainer(CustomContainer &&other) noexcept
    : mBegin(std::exchange(other.mBegin)),
      mLast(std::move(other.mLast)),
      mEnd(std::move(other.mEnd)),
      mAllocator(other.mAllocator),
      mSize(std::exchange(other.mSize, 0)) {
    }

    CustomContainer(const CustomContainer &other)
    : mBegin(nullptr),
      mAllocator(decltype(other.mAllocator)()),
      mSize(other.mSize) {
        for (auto field: other) {
            auto prev = mLast;
            mLast = mAllocator.allocate(1);
            mAllocator.construct(&mLast->value, field);
            if (mBegin == nullptr) {
                mBegin = mLast;
                mBegin->prev = nullptr;
                mBegin->next = mLast;
            } else {
                prev->next = mLast;
                mLast->prev = prev;
                mLast->next = &mEnd;
            }
            mEnd.prev = mLast;
        }
    }

    ~CustomContainer() {
        while (mLast->prev != nullptr) {
            mAllocator.destroy(&mLast->value);
            mLast = mLast->prev;
            mAllocator.deallocate(mLast->next, 1);
            mLast->next = nullptr;
        }
        mAllocator.destroy(&mBegin->value);
        mAllocator.deallocate(mBegin, 1);
    }

    CustomContainer &operator=(const CustomContainer &other) {
        if (!empty()) {
            this->~CustomContainer();
        } else {
            mAllocator.deallocate(mBegin, mSize);
        }
        new(this) CustomContainer(other);
        return *this;
    }
    bool operator==(const CustomContainer &other) const {
        if (mSize != other.mSize) {
            return false;
        }
        Field *it1 = mBegin;
        Field *it2 = other.mBegin;
        while (it1 != mLast && it2 != mLast) {
            if (it1->value != it2->value) {
                return false;
            }
            it1 = it1->next;
            it2 = it2->next;
        }
        return true;
    }
    bool operator!=(const CustomContainer &other) const {
        return !(*this == other);
    }
    reference operator[](size_t n) {
        if (n > mSize -1) {
            throw std::out_of_range("Index out of range");
        }
        auto it = mBegin;
        if ((mSize / 2) > n) {
            for (size_t i = 0; i < n; ++i) {
                it = it->next;
            }
        } else {
            it = &mEnd;
            for (size_t i = 0; i < mSize - n; ++i) {
                it = it->prev;
            }
        }
        return it->value;
    }

    iterator begin() {
        return iterator(mBegin);
    }
    const_iterator begin() const {
        return const_iterator(mBegin);
    }
    const_iterator cbegin() const {
        return const_iterator(mBegin);
    }
    iterator end() {
        return iterator(mLast->next);
    }
    const_iterator end() const {
        return const_iterator(mLast->next);
    }
    const_iterator cend() const {
        return const_iterator(mLast->next);
    }
    void swap(CustomContainer &other) {
        std::swap(mBegin, other.mBegin);
        std::swap(mLast, other.mLast);
        std::swap(mEnd, other.mEnd);
        std::swap(mAllocator, other.mAllocator);
        std::swap(mSize, other.mSize);
    }
    size_t size() const {
        return mSize;
    }
    size_t max_size() const {
        return mAllocator.max_size();
    }
    bool empty() const {
        return mSize == 0;
    }
    template<class ...Args>
    void emplace_back(Args&&... args) {
        auto prev = mLast;
        mLast = mAllocator.allocate(1);
        mAllocator.construct(&mLast->value, std::forward<Args>(args)...);
        if (mBegin == nullptr) {
            mBegin = mLast;
            mBegin->prev = nullptr;
            mBegin->next = mLast;
        } else {
            prev->next = mLast;
            mLast->prev = prev;
            mLast->next = &mEnd;
        }
        mEnd.prev = mLast;
        mSize+=1;
    }
private:
    Field *mBegin;
    Field *mLast;
    Field mEnd;
    FieldAllocator mAllocator;
    size_t mSize;
};
