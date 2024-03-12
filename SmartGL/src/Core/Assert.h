#pragma once

#ifndef SIGTRAP // SIGTRAP is not defined on Windows
#define SIGTRAP 5
#endif

#ifndef raise // raise is not defined on Windows
#define raise(x) 0
#endif

#if SMART_PLATFORM_WINDOWS
#define SMART_BREAK() __debugbreak()
#else
#define SMART_BREAK() raise(SIGTRAP)
#endif

#ifdef SMART_PLATFORM_LINUX
#define SMART_ASSERT(condition, ...)                                                                      \
    do                                                                                                    \
    {                                                                                                     \
        if (!(condition))                                                                                 \
        {                                                                                                 \
            fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", #condition, __FILE__, __LINE__);  \
            SMART_BREAK();                                                                                \
            exit(1);                                                                                      \
        }                                                                                                 \
    } while (0)
#else
#define SMART_ASSERT(condition, ...)                                                                      \
    do                                                                                                    \
    {                                                                                                     \
        if (!(condition))                                                                                 \
        {                                                                                                 \
            fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", #condition, __FILE__, __LINE__);  \
            SMART_BREAK();                                                                                \
        }                                                                                                 \
    } while (0)
#endif
