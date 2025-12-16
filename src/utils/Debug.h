#pragma once

#if defined(_MSC_VER)
#define DEBUG_BREAK() __debugbreak() // MSVC specific
#elif defined(__GNUC__) || defined(__clang__)
#define DEBUG_BREAK() __builtin_trap()
#else
#define DEBUG_BREAK() abort()
#endif

#ifdef _DEBUG
#define ASSERT(x, msg) \
        if (!(x)) { \
            fprintf(stderr, "ASSERTION FAILED: %s\nFile: %s\nLine: %d\nMessage: %s\n", #x, __FILE__, __LINE__, msg); \
            DEBUG_BREAK(); \
        }
#else
#define ASSERT(x, msg) void(0)
#endif