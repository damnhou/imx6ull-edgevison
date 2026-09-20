#include "system_metrics.h"

#if defined(__linux__) || defined(__APPLE__)
#include <sys/resource.h>
#endif

long peakResidentSetKb()
{
#if defined(__linux__) || defined(__APPLE__)
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

