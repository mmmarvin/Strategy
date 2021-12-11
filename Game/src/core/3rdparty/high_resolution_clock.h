#ifndef GUARD_MAR_high_resolution_clock_H
#define GUARD_MAR_high_resolution_clock_H

// standard headers
#include <chrono>
#include <ctime>
#include <tuple>

// custom headers
#include "time.h"

namespace mar
{
    class high_resolution_clock
    {
    public:
      high_resolution_clock();

      time get_elapsed() const;
      time restart();

    private:
      std::pair<time, std::chrono::high_resolution_clock::time_point> get_elapsed_impl() const;
      time restart_impl();

      std::chrono::high_resolution_clock::time_point  m_startTime;
    };

    class high_resolution_stopwatch : public high_resolution_clock
    {
    public:
        high_resolution_stopwatch();

        void start();
        void stop() noexcept;
        time restart();

        bool is_running() const noexcept { return m_isrunning; }

    private:
        bool                                            m_isrunning;
    };
}

#endif // GUARD_MAR_high_resolution_clock_H
