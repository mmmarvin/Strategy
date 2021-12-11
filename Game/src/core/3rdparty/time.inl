// custom headers
#include "time.h"

namespace mar
{
    template<class T>
    time& operator*=(time& lhs, const T& rhs)
    {
        lhs.m_value *= static_cast<time::nanoseconds_value_type>(rhs);
        return lhs;
    }

    template<class T>
    time operator*(const time& lhs, const T& rhs)
    {
        auto ret = lhs;
        ret *= rhs;

        return ret;
    }

    template<class T>
    time& operator/=(time& lhs, const T& rhs)
    {
        lhs.m_value /= static_cast<time::nanoseconds_value_type>(rhs);
        return lhs;
    }

    template<class T>
    time operator/(const time& lhs, const T& rhs)
    {
        auto ret = lhs;
        ret /= rhs;

        return ret;
    }
}
