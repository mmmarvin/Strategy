#include "fixed_sized_array.h"

// standard headers
#include <algorithm>

// custom headers
#include "temporary_array.h"

namespace mar
{
namespace container
{
    template<class T, std::size_t Size, class AllocatorT>
    template<class>
    fixed_sized_array<T, Size, AllocatorT>::fixed_sized_array(const allocator_type& alloc) :
        allocator_type(alloc),
        m_data(MAR_NULL)
    {
        temporary_array<T, AllocatorT> temp(Size, T(), alloc);
        m_data = temp.transfer();
    }

    template<class T, std::size_t Size, class AllocatorT>
    fixed_sized_array<T, Size, AllocatorT>::fixed_sized_array(const fixed_sized_array& rhs) :
        allocator_type(rhs),
        m_data(MAR_NULL)
    {
        temporary_array<T, AllocatorT> temp(rhs.m_data, rhs.m_data + Size, rhs);
        m_data = temp.transfer();
    }

    template<class T, std::size_t Size, class AllocatorT>
    fixed_sized_array<T, Size, AllocatorT>::fixed_sized_array(fixed_sized_array&& rhs) :
        allocator_type(std::move(rhs.allocator())),
        m_data(MAR_NULL)
    {
        swap_data(rhs);
    }

    template<class T, std::size_t Size, class AllocatorT>
    fixed_sized_array<T, Size, AllocatorT>& fixed_sized_array<T, Size, AllocatorT>::operator=(const fixed_sized_array& rhs)
    {
        fixed_sized_array<T, Size, AllocatorT> temp(rhs);
        swap(temp);

        return *this;
    }

    template<class T, std::size_t Size, class AllocatorT>
    fixed_sized_array<T, Size, AllocatorT>& fixed_sized_array<T, Size, AllocatorT>::operator=(fixed_sized_array&& rhs)
    {
        swap(rhs);

        return *this;
    }

    template<class T, std::size_t Size, class AllocatorT>
    fixed_sized_array<T, Size, AllocatorT>::fixed_sized_array(std::initializer_list<T> il, const allocator_type& alloc) :
        allocator_type(alloc),
        m_data(MAR_NULL)
    {
        assert(il.size() == Size);
        temporary_array<T, AllocatorT> temp(il.begin(), il.end(), alloc);
        m_data = temp.transfer();
    }

    template<class T, std::size_t Size, class AllocatorT>
    fixed_sized_array<T, Size, AllocatorT>::fixed_sized_array(const value_type& v, const allocator_type& alloc) :
        allocator_type(alloc),
        m_data(MAR_NULL)
    {
        temporary_array<T, AllocatorT> temp(Size, v, alloc);
        m_data = temp.transfer();
    }

    template<class T, std::size_t Size, class AllocatorT>
    fixed_sized_array<T, Size, AllocatorT>::~fixed_sized_array()
    {
        if(m_data) {
            for(decltype(Size) i = 0; i < Size; ++i) {
                allocator().destroy(m_data + i);
            }
            allocator().deallocate(m_data, Size);
        }
    }

    template<class T, std::size_t Size, class AllocatorT>
    void fixed_sized_array<T, Size, AllocatorT>::swap(fixed_sized_array& other) MAR_NO_EXCEPT
    {
        assert(allocator_trait::propagate_on_container_swap::value || allocator() == other.allocator());
        std::swap(allocator(), other.allocator());
        swap_data(other);
    }

    template<class T, std::size_t Size, class AllocatorT>
    void fixed_sized_array<T, Size, AllocatorT>::swap_data(fixed_sized_array& other) MAR_NO_EXCEPT
    {
        std::swap(m_data, other.m_data);
    }
}
}
