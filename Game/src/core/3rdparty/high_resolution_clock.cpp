// custom headers
#include "high_resolution_clock.h"

namespace mar
{
    high_resolution_clock::high_resolution_clock() :
        m_startTime(std::chrono::high_resolution_clock::now())
    {
    }

    time high_resolution_clock::get_elapsed() const
    {
        return get_elapsed_impl().first;
    }

    time high_resolution_clock::restart()
    {
        return restart_impl();
    }

    std::pair<time, std::chrono::high_resolution_clock::time_point> high_resolution_clock::get_elapsed_impl() const
    {
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        std::chrono::high_resolution_clock::time_point prev = m_startTime;

        time elapsed;
        elapsed += time(time::nanoseconds(std::chrono::duration_cast<std::chrono::nanoseconds>(now - prev).count()));

        return { elapsed, now };
    }

    time high_resolution_clock::restart_impl()
    {
        auto current_time = get_elapsed_impl();
        m_startTime = current_time.second;

        return current_time.first;
    }

    high_resolution_stopwatch::high_resolution_stopwatch() :
        m_isrunning(false)
    {
    }

    void high_resolution_stopwatch::start()
    {
        if(!m_isrunning) {
            restart();
            m_isrunning = true;
        }
    }

    void high_resolution_stopwatch::stop() noexcept
    {
        m_isrunning = false;
    }

    time high_resolution_stopwatch::restart()
    {
        if(!m_isrunning)
          m_isrunning = true;

        return high_resolution_clock::restart();
    }
}
