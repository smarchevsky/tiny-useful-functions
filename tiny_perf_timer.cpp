#include <chrono>
#include <stdio.h>
class Logger {
#ifdef _WIN32
#define lock_stream(f) _lock_file(f)
#define unlock_stream(f) _unlock_file(f)
#else
#define lock_stream(f) flockfile(f)
#define unlock_stream(f) funlockfile(f)
#endif

    using clock = std::chrono::high_resolution_clock;
    friend class ScopeTimer;

    FILE* _file;
    std::chrono::time_point<clock> _start;
    int _indentation;

public:
    Logger()
    {
#ifdef _WIN32
        fopen_s(&_file, "timepoints.txt", "w");
#else
        _file = fopen("timepoints.txt", "w");
#endif
        if (_file) {
            fprintf(_file, "[LOGGING START]\n");
            fflush(_file);
        }
        _indentation = 0;
        _start = clock::now();
    }

    template <bool WithIdentation>
    void log(const char* format, ...)
    {
        if (!_file)
            return;

        va_list args;
        va_start(args, format);
        lock_stream(_file);
        if constexpr (WithIdentation) {
            for (int i = 0; i < _indentation; ++i)
                fputc(' ', _file);
        }
        vfprintf(_file, format, args);
        fputc('\n', _file);
        unlock_stream(_file);
        va_end(args);
    }

    void logTimepoint(const char* str)
    {
        const float duration = std::chrono::duration<float>(clock::now() - _start).count();
        log<false>("Timepoint: [%s] %f", str, duration * 1000);
    }

    ~Logger()
    {
        if (_file) {
            lock_stream(_file);
            fprintf(_file, "[LOGGING END]\n");
            unlock_stream(_file);
            fclose(_file);
            _file = nullptr;
        }
    }

    static Logger& get()
    {
        static Logger instance;
        return instance;
    }
};

class ScopeTimer {
    using clock = std::chrono::high_resolution_clock;
    std::chrono::time_point<clock> _start;
    const char* _timerName;

public:
    ScopeTimer(const ScopeTimer&) = delete;
    ScopeTimer(ScopeTimer&&) = delete;
    ScopeTimer(const char* str)
        : _timerName(str)
    {
        Logger::get()._indentation += 2;
        Logger::get().log<true>("%s, start", _timerName);
        _start = clock::now();
    }

    ~ScopeTimer()
    {
        const float duration = std::chrono::duration<float>(clock::now() - _start).count();
        Logger::get().log<true>("%s, finished %.3f ms", _timerName, duration * 1000);
        Logger::get()._indentation -= 2;
    }

#define TIMEPOINT_RAII(str) ScopeTimer scopeTimer(str);
#define TIMEPOINT(str) Logger::get().logTimepoint(str);
};
