#include <chrono>
class Timer {
    using clock = std::chrono::high_resolution_clock;

    FILE* m_file;
    std::chrono::time_point<clock> m_start;

public:
    Timer()
    {
        fopen_s(&m_file, "timepoints.txt", "w");
        fprintf(m_file, "[TIMER START]\n");
        m_start = clock::now();
    }

    ~Timer()
    {
        logTimepoint("TIMER END");
        std::fclose(m_file);
    }

    void restart() { m_start = clock::now(); }

    void logTimepoint(const char* str)
    {
        fprintf(m_file, "[%s]: %.3f ms\n", str, std::chrono::duration<float>(clock::now() - m_start).count() * 1000);
    }

    static Timer& get()
    {
        static Timer instance;
        return instance;
    }

    struct TimePoint {
        const char* _timePointName;
        TimePoint(const char* funcName)
            : _timePointName(funcName)
        {
            Timer::get().restart();
        }
        ~TimePoint() { Timer::get().logTimepoint(_timePointName); }
    };

#define TIME_POINT_RAII(str) Timer::TimePoint localTimePoint(str);
#define TIMEPOINT(str) Timer::get().logTimepoint(str);
};
