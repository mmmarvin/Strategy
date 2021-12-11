#ifndef GUARD_MAR_container_temporary_array_H
#define GUARD_MAR_container_temporary_array_H

// standard headers
#include <memory>
#include <type_traits>

#ifndef MAR_NULL
#if __cplusplus >= 201103L
#define MAR_NULL nullptr
#else
#define MAR_NULL 0
#endif // C++11
#endif // MAR_NULL

#ifndef MAR_NO_EXCEPT
#if __cplusplus >= 201103L
#define MAR_NO_EXCEPT noexcept
#else
#define MAR_NO_EXCEPT
#endif // C++11
#endif // MAR_NO_EXCEPT

namespace mar
{
namespace container
{
    template<class T,
             class AllocatorT = std::allocator<T>>
    class temporary_array : AllocatorT
    {
    public:
        typedef AllocatorT                                  allocator_type;
        typedef std::allocator_traits<AllocatorT>           allocator_trait;
        typedef typename allocator_trait::value_type        value_type;
        typedef typename allocator_trait::pointer           pointer;
        typedef typename allocator_trait::const_pointer     const_pointer;
        typedef typename allocator_type::reference          reference;
        typedef typename allocator_type::const_reference    const_reference;
        typedef typename allocator_trait::size_type         size_type;

        temporary_array(size_type n, const allocator_type& alloc = allocator_type());
        temporary_array(size_type n, const value_type& value, const allocator_type& alloc = allocator_type());
        temporary_array(const temporary_array& rhs);
        temporary_array(temporary_array&& rhs) MAR_NO_EXCEPT(std::is_nothrow_move_constructible<T>());
        temporary_array& operator=(const temporary_array& rhs);
        temporary_array& operator=(temporary_array&& rhs) MAR_NO_EXCEPT(std::is_nothrow_move_assignable<T>());
        temporary_array(std::initializer_list<T> il, const allocator_type& alloc = allocator_type());
        template<class IteratorT> temporary_array(IteratorT first, IteratorT last, const allocator_type& alloc = allocator_type());
        ~temporary_array() MAR_NO_EXCEPT;

        template<class... Args> void emplace_back(Args&&... args);
        pointer transfer() MAR_NO_EXCEPT;

        void swap(temporary_array& other) MAR_NO_EXCEPT;

        size_type capacity() const MAR_NO_EXCEPT { return m_capacity; }
        size_type size() const MAR_NO_EXCEPT { return m_size; }

    private:
        template<class... Args> void emplace_back_impl(Args&&... args);

        void swap_data(temporary_array& other) MAR_NO_EXCEPT;

        allocator_type& allocator() MAR_NO_EXCEPT { return *this; }
        const allocator_type& allocator() const MAR_NO_EXCEPT { return *this; }

        pointer     m_data;
        size_type   m_size;
        size_type   m_capacity;
    };
}
}

// definitions
#include "temporary_array.inl"

#endif // GUARD_MAR_container_temporary_array_H
