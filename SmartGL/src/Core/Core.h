#pragma once

#include <memory>

#define BIT(x) (1 << x)

template <typename T>
int SIGN(T val)
{
    return (T(0) < val) - (val < T(0));
}

#ifdef SMART_PLATFORM_LINUX
#define LINUX_MALLOC(length, size) malloc((length) * (size))
#define MALLOC(length, size) LINUX_MALLOC(length, size)
#elif defined(SMART_PLATFORM_WINDOWS)
#define WINDOWS_MALLOC(length, size) _malloca((length) * (size))
#define MALLOC(length, size) WINDOWS_MALLOC(length, size)
#else
#define MALLOC(length, size) malloc((length) * (size))
#endif
