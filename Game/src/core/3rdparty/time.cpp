// custom headers
#include "time.h"

namespace mar
{
    time& operator+=(time& lhs, const time& rhs)
    {
        lhs.m_value += rhs.m_value;
        return lhs;
    }

    time operator+(const time& lhs, const time& rhs)
    {
        auto ret = lhs;
        ret += rhs;

        return ret;
    }

    time& operator-=(time& lhs, const time& rhs)
    {
        lhs.m_value -= rhs.m_value;
        return lhs;
    }

    time operator-(const time& lhs, const time& rhs)
    {
        auto ret = lhs;
        ret -= rhs;

        return ret;
    }

    bool operator<(const time& lhs, const time& rhs)
    {
        return lhs.m_value < rhs.m_value;
    }

    bool operator>(const time& lhs, const time& rhs)
    {
        return lhs.m_value > rhs.m_value;
    }

    bool operator==(const time& lhs, const time& rhs)
    {
        return lhs.m_value == rhs.m_value;
    }
}
