/**
 * @file obj_allocator.hpp
 * @author zhoutong (zhoutotong@qq.com)
 * @brief 
 * @version 0.1
 * @date 2023-11-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once
#include <string>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <unordered_map>

#include <coin-data/local/impl/shm_memory.hpp>


namespace coin::data::__inner
{

template<typename T>
class ObjAllocator
{
    static_assert(not std::is_polymorphic<T>::value, "you can only allocator object that is not polymorphic.");

public:
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using const_pointer = const T*;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    template<typename _Tp1>
	struct rebind
	{ typedef ObjAllocator<_Tp1> other; };

public:
    ObjAllocator() noexcept {}
    
    ObjAllocator(const ObjAllocator& __a) noexcept {}

    template<typename U>
    ObjAllocator(const ObjAllocator<U>&) noexcept {}

    template<typename U>
    ObjAllocator(ObjAllocator<U>&) noexcept {}
    
    ~ObjAllocator() noexcept {}

    ObjAllocator& operator = (const ObjAllocator&) = default;

    pointer       address ( reference x )       const noexcept { return &x; }
    const_pointer address ( const_reference x ) const noexcept { return &x; }

    pointer allocate (size_type n)
    {
        pointer ptr = (pointer)::coin::data::__inner::ShmMemory::mem().obj_malloc(n * sizeof(T));
        return ptr;
    }

    void deallocate (pointer p, size_type n)
    {
        ::coin::data::__inner::ShmMemory::mem().obj_free(p);
    }

    size_type max_size() const noexcept { return __LONG_MAX__; }

    template <class U, class... Args>  void construct (U* p, Args&&... args)
    {
        new (p)U(std::forward<Args>(args)...);
    }
    template <class U>  void destroy (U* p)
    {
        p->~U();
    }
};

using ShmObjString = std::basic_string<char, std::char_traits<char>, ::coin::data::__inner::ObjAllocator<char> >;

template<typename EleT>
using ShmObjVector = std::vector<EleT, ::coin::data::__inner::ObjAllocator<EleT> >;

template<typename EleT>
using ShmObjList = std::list<EleT, ::coin::data::__inner::ObjAllocator<EleT> >;

template<typename EleT>
using ShmObjDeque = std::deque<EleT, ::coin::data::__inner::ObjAllocator<EleT> >;

template<typename KeyT, typename ValT, typename _Compare = std::less<KeyT>>
using ShmObjMap = std::map<KeyT, ValT, _Compare, ::coin::data::__inner::ObjAllocator< std::pair<const KeyT, ValT> > >;

template<typename KeyT, typename ValT, typename _Hash = std::hash<KeyT>, typename _Pred = std::equal_to<KeyT>>
using ShmObjUnorderedMap = std::unordered_map<KeyT, ValT,
    _Hash, _Pred, ::coin::data::__inner::ObjAllocator< std::pair<const KeyT, ValT> > >;


} // namespace coin::data::__inner
