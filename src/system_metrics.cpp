#include "system_metrics.h"

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <psapi.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <sys/resource.h>
#endif

long peakResidentSetKb()
{
#if defined(_WIN32)
    PROCESS_MEMORY_COUNTERS counters;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &counters, sizeof(counters)))
        return static_cast<long>(counters.PeakWorkingSetSize / 1024);
#elif defined(__linux__) || defined(__APPLE__)
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
#if defined(__APPLE__)
        return static_cast<long>(usage.ru_maxrss / 1024);
#else
        return static_cast<long>(usage.ru_maxrss);
#endif
    }
#endif
    return 0;
}
