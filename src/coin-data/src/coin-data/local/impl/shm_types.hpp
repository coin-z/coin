/**
 * @file shm_types.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-11-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once
#include <sys/types.h>
#include <signal.h>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <set>
#include <unordered_map>
#include <typeinfo>
#include <memory>
#include <functional>
#include <optional>

#include <coin-data/local/impl/allocator.hpp>

namespace coin::data
{
using ProcessMutex   = __inner::ShmMemory::ProcessMutex;
using ProcessRWMutex = __inner::ShmMemory::ProcessRWMutex;

using ShmString = std::basic_string<char, std::char_traits<char>
                                    , coin::data::__inner::Allocator<char> >;

inline std::string toStdString(const ShmString& str)
{
    std::string s;
    s.resize(str.size());
    memcpy(s.data(), str.data(), str.size());
    return s;
}
inline ShmString fromStdString(const std::string& str)
{
    ShmString s;
    s.resize(str.size());
    memcpy(s.data(), str.data(), str.size());
    return s;
}

template<typename EleT>
using ShmVector = std::vector<EleT, coin::data::__inner::Allocator<EleT>>;

template<typename EleT>
using ShmList = std::list<EleT, coin::data::__inner::Allocator<EleT>>;

template<typename EleT>
using ShmDeque = std::deque<EleT, coin::data::__inner::Allocator<EleT>>;

template<typename KeyT, typename ValT, typename _Compare = std::less<KeyT>>
using ShmMap = std::map<KeyT, ValT,
    _Compare, coin::data::__inner::Allocator< std::pair<const KeyT, ValT> > >;

template<typename KeyT, typename ValT, typename _Hash = std::hash<KeyT>, typename _Pred = std::equal_to<KeyT>>
using ShmUnorderedMap = std::unordered_map<KeyT, ValT,
    _Hash, _Pred, ::coin::data::__inner::Allocator< std::pair<const KeyT, ValT> > >;

template<typename KeyT, typename _Compare = std::less<KeyT>>
using ShmSet = std::set<KeyT, _Compare, coin::data::__inner::Allocator<KeyT> > ;

template<typename MutexT>
class ProcessLockGuard
{
public:
    ProcessLockGuard(MutexT& m) : m_(m)
    {
        m_.lock();
    }

    ProcessLockGuard(MutexT& m, int ms) : m_(m)
    {
        while(0 != m_.timedlock(ms))
        {
            if(0 != kill(m_.holder(), 0))
            {
                std::cout << "process not exist, unlock." << std::endl;
                m_.unlock();
                continue;
            }
            else
            {
                continue;
            }
        }
        if(m_.holder() == 0)
        {
            fprintf(stderr, "holder is zero\n");
            abort();
        }
    }

    ~ProcessLockGuard()
    {
        m_.unlock();
    }

private:
    MutexT& m_;
};

template<typename MutexT>
class ProcessRLockGuard
{
public:

    ProcessRLockGuard(MutexT& m) : m_(m)
    {
        m_.rlock();
    }

    ~ProcessRLockGuard()
    {
        m_.unlock();
    }

private:
    MutexT& m_;
};

template<typename MutexT>
class ProcessWLockGuard
{
public:
    ProcessWLockGuard(MutexT& m) : m_(m)
    { m_.wlock(); }

    ~ProcessWLockGuard()
    { m_.unlock(); }

private:
    MutexT& m_;
};

namespace __inner
{

template<typename T>
class SharedCircularBuffer
{
    class InOut
    {
    public:
        InOut(const std::function<void()>& in, const std::function<void()>& out) : out_(out)
        { if(in) in(); }
        ~InOut()
        { if(out_) out_(); }
    private:
        const std::function<void()> out_;
    };
public:
    SharedCircularBuffer() = delete;
    SharedCircularBuffer(const SharedCircularBuffer&) = delete;
    SharedCircularBuffer& operator = (const SharedCircularBuffer&) = delete;
    SharedCircularBuffer(const size_t& size) : head_(0), tail_(0), total_size_(size), buffer_(size)
    {
        buffer_.shrink_to_fit();
    }
    ~SharedCircularBuffer()
    {
        buffer_.clear();
    }

    void push_back(const T& val)
    {
        ProcessLockGuard<ProcessMutex> lock(mutex_, 100);
        InOut io(nullptr, [this](){
            tail_ += 1;
            if(tail_ - head_ >= total_size_)
            {
                head_ += 1;
            }
        });
        size_t idx = (tail_) % total_size_;
        buffer_[idx] = val;
    }

    [[nodiscard]] T& operator [] (const size_t& idx)
    {
        ProcessLockGuard<ProcessMutex> lock(mutex_, 100);
        auto i = idx;
        if(i < head_)
        {
            i = head_;
        }
        else if(i >= tail_)
        {
            i = (tail_ - 1);
        }
        return buffer_[i % total_size_];
    }

    [[nodiscard]] T copy(const size_t& idx)
    {
        ProcessLockGuard<ProcessMutex> lock(mutex_, 100);
        auto i = idx;
        if(i < head_)
        {
            i = head_;
        }
        else if(i >= tail_)
        {
            i = (tail_ - 1);
        }
        return buffer_[i % total_size_];
    }

    size_t head() noexcept
    {
        ProcessLockGuard<ProcessMutex> lock(mutex_, 100);
        return head_;
    }

    size_t tail() noexcept
    {
        ProcessLockGuard<ProcessMutex> lock(mutex_, 100);
        return tail_;
    }

    void clear() noexcept
    {
        buffer_.clear();
    }
    size_t size() noexcept
    {
        ProcessLockGuard<ProcessMutex> lock(mutex_, 100);
        return buffer_.size();
    }

private:
    size_t head_;
    size_t tail_;
    const size_t total_size_;
    ShmVector<T> buffer_;
    ProcessMutex mutex_;
};
} // namespace __inner

} // namespace coin::data
