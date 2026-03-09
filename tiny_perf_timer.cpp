#include <chrono>
class Timer {
    FILE* m_file;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;

public:
    Timer()
    {
        fopen_s(&m_file, "timepoints.txt", "w");
        m_start = std::chrono::high_resolution_clock::now();
        fprintf(m_file, "[TIMER START]\n");
    }

    ~Timer()
    {
        timepoint("TIMER END", "");
        std::fclose(m_file);
    }

    void timepoint(const char* str1, const char* str2 = nullptr)
    {
        std::chrono::duration<double, std::milli> elapsed = std::chrono::high_resolution_clock::now() - m_start;
        fprintf(m_file, "[%s %s]: %.3f ms\n", str1, str2, elapsed.count());
    }

    void restart() { m_start = std::chrono::high_resolution_clock::now(); }

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
        ~TimePoint() { Timer::get().timepoint(_timePointName, "function duration"); }
    };

#define TIME_POINT_RAII(str) Timer::TimePoint localTimePoint(str);
#define TIMEPOINT(str1, str2) Timer::get().timepoint(str1, str2);
};
