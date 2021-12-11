#include "temporary_array.h"

// standard headers
#include <assert.h>
#include <algorithm>
#include <functional>

namespace mar
{
namespace container
{
    template<class T, class AllocatorT>
    temporary_array<T, AllocatorT>::temporary_array(size_type n, const allocator_type& alloc) :
        allocator_type(alloc),
        m_data(n > 0 ? allocator().allocate(n) : MAR_NULL),
        m_size(0),
        m_capacity(n)
    {
    }

    template<class T, class AllocatorT>
    temporary_array<T, AllocatorT>::temporary_array(size_type n, const value_type& value, const allocator_type& alloc) :
        allocator_type(alloc),
        m_data(n > 0 ? allocator().allocate(n) : MAR_NULL),
        m_size(0),
        m_capacity(n)
    {
        while(m_size < m_capacity)
            emplace_back_impl(value);
    }

    template<class T, class AllocatorT>
    temporary_array<T, AllocatorT>::temporary_array(const temporary_array& rhs) :
        temporary_array<T, AllocatorT>(rhs.m_data, rhs.m_data + rhs.m_size, rhs)
    {
    }

    template<class T, class AllocatorT>
    temporary_array<T, AllocatorT>::temporary_array(temporary_array&& rhs) MAR_NO_EXCEPT(std::is_nothrow_move_constructible<T>()) :
        allocator_type(std::move(rhs.allocator())),
        m_data(MAR_NULL),
        m_size(0),
        m_capacity(0)
    {
        swap_data(rhs);
    }

    template<class T, class AllocatorT>
    temporary_array<T, AllocatorT>& temporary_array<T, AllocatorT>::operator=(const temporary_array& rhs)
    {
        temporary_array<T, AllocatorT> temp(rhs);
        swap(temp);

        return *this;
    }

    template<class T, class AllocatorT>
    temporary_array<T, AllocatorT>& temporary_array<T, AllocatorT>::operator=(temporary_array&& rhs) MAR_NO_EXCEPT(std::is_nothrow_move_assignable<T>())
    {
        swap(rhs);
        return *this;
    }

    template<class T, class AllocatorT>
    temporary_array<T, AllocatorT>::temporary_array(std::initializer_list<T> il, const allocator_type& alloc) :
        temporary_array<T, AllocatorT>(il.begin(), il.end(), alloc)
    {
    }

    template<class T, class AllocatorT>
    template<class IteratorT>
    temporary_array<T, AllocatorT>::temporary_array(IteratorT first, IteratorT last, const allocator_type& alloc) :
        allocator_type(alloc),
        m_data(MAR_NULL),
        m_size(0),
        m_capacity(0)
    {
        auto distance = std::distance(first, last);
        if(distance > 0) {
            auto d = static_cast<size_type>(distance);
            m_data = allocator().allocate(d);
            m_capacity = d;

            // copy the contents
            auto it = first;
            while(it != last) {
                emplace_back_impl(*it);
                ++it;
            }
        }
    }

    template<class T, class AllocatorT>
    temporary_array<T, AllocatorT>::~temporary_array() MAR_NO_EXCEPT
    {
        if(m_data) {
            for(size_type i = 0; i < m_size; ++i) {
                allocator().destroy(m_data + i);
            }
            allocator().deallocate(m_data, m_capacity);
        }
    }

    template<class T, class AllocatorT>
    template<class... Args>
    void temporary_array<T, AllocatorT>::emplace_back(Args&&... args)
    {
        bool enough_space = m_size < m_capacity;
        assert(enough_space);
        if(enough_space) {
            emplace_back_impl(std::forward<Args>(args)...);
        }
    }

    template<class T, class AllocatorT>
    typename temporary_array<T, AllocatorT>::pointer temporary_array<T, AllocatorT>::transfer() MAR_NO_EXCEPT
    {
        pointer temp = MAR_NULL;
        std::swap(m_data, temp);
        m_size = 0;
        m_capacity = 0;

        return temp;
    }

    template<class T, class AllocatorT>
    void temporary_array<T, AllocatorT>::swap(temporary_array& other) MAR_NO_EXCEPT
    {
        assert(allocator_trait::propagate_on_container_swap::value || allocator() == other.allocator());
        std::swap(allocator(), other.allocator());
        swap_data(other);
    }

    template<class T, class AllocatorT>
    template<class... Args>
    void temporary_array<T, AllocatorT>::emplace_back_impl(Args&&... args)
    {
        allocator().construct(m_data + m_size, std::forward<Args>(args)...);
        ++m_size;
    }

    template<class T, class AllocatorT>
    void temporary_array<T, AllocatorT>::swap_data(temporary_array& other) MAR_NO_EXCEPT
    {
        std::swap(m_data, other.m_data);
        std::swap(m_size, other.m_size);
        std::swap(m_capacity, other.m_capacity);
    }
}
}
