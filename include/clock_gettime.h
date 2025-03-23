#if !defined(_AMD64_) || !defined(_X86_)
#define _AMD64_ 1
#define _WIN64 1
#endif

#ifndef __clockid_t_defined
typedef int clockid_t;
#define __clockid_t_defined 1
#define CLOCK_MONOTONIC 1
#define CLOCK_REALTIME 2
#endif

#ifndef _TIMESPEC_DEFINED
#define _TIMESPEC_DEFINED
struct timespec {
  long tv_sec;  /* Seconds */
  long tv_nsec; /* Nanoseconds */
};

struct itimerspec {
  struct timespec it_interval; /* Timer period */
  struct timespec it_value;    /* Timer expiration */
};
#endif

#include <profileapi.h>
// #include <time.h>
#include <sysinfoapi.h>
#include <winnt.h>

#define MS_PER_SEC 1000ULL // MS = milliseconds
#define US_PER_MS 1000ULL  // US = microseconds
#define HNS_PER_US 10ULL   // HNS = hundred-nanoseconds (e.g., 1 hns = 100 ns)
#define NS_PER_US 1000ULL

#define HNS_PER_SEC (MS_PER_SEC * US_PER_MS * HNS_PER_US)
#define NS_PER_HNS (100ULL) // NS = nanoseconds
#define NS_PER_SEC (MS_PER_SEC * US_PER_MS * NS_PER_US)

static inline int clock_gettime_monotonic(struct timespec *tv) {
  static LARGE_INTEGER ticksPerSec;
  LARGE_INTEGER ticks;

  if (!ticksPerSec.QuadPart) {
    QueryPerformanceFrequency(&ticksPerSec);
    if (!ticksPerSec.QuadPart) {
      errno = ENOTSUP;
      return -1;
    }
  }

  QueryPerformanceCounter(&ticks);

  tv->tv_sec = (long)(ticks.QuadPart / ticksPerSec.QuadPart);
  tv->tv_nsec = (long)(((ticks.QuadPart % ticksPerSec.QuadPart) * NS_PER_SEC) /
                       ticksPerSec.QuadPart);

  return 0;
}

static inline int clock_gettime_realtime(struct timespec *tv) {
  FILETIME ft;
  ULARGE_INTEGER hnsTime;

  GetSystemTimePreciseAsFileTime(&ft);

  hnsTime.LowPart = ft.dwLowDateTime;
  hnsTime.HighPart = ft.dwHighDateTime;

  // To get POSIX Epoch as baseline, subtract the number of hns intervals from
  // Jan 1, 1601 to Jan 1, 1970.
  hnsTime.QuadPart -= (11644473600ULL * HNS_PER_SEC);

  // modulus by hns intervals per second first, then convert to ns, as not to
  // lose resolution
  tv->tv_nsec = (long)((hnsTime.QuadPart % HNS_PER_SEC) * NS_PER_HNS);
  tv->tv_sec = (long)(hnsTime.QuadPart / HNS_PER_SEC);

  return 0;
}

static inline int clock_gettime(clockid_t type, struct timespec *tp) {
  if (type == CLOCK_MONOTONIC) {
    return clock_gettime_monotonic(tp);
  } else if (type == CLOCK_REALTIME) {
    return clock_gettime_realtime(tp);
  }

  errno = ENOTSUP;
  return -1;
}

static inline void timespec_diff(struct timespec *start, struct timespec *end,
                                 struct timespec *result) {
  if ((end->tv_nsec - start->tv_nsec) < 0) {
    // Borrow 1 second and adjust nanoseconds
    result->tv_sec = end->tv_sec - start->tv_sec - 1;
    result->tv_nsec = (NS_PER_SEC + end->tv_nsec) - start->tv_nsec;
  } else {
    result->tv_sec = end->tv_sec - start->tv_sec;
    result->tv_nsec = end->tv_nsec - start->tv_nsec;
  }
}
