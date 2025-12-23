#pragma once

//#define GAMEBOY_DOCTOR

#if defined(_MSC_VER)
#define DEBUG_BREAK() __debugbreak() // MSVC specific
#elif defined(__GNUC__) || defined(__clang__)
#define DEBUG_BREAK() __builtin_trap()
#else
#define DEBUG_BREAK() abort()
#endif

#ifdef _DEBUG
#define ASSERT(x, fmt, ...) \
    do { \
        if (!(x)) { \
            fprintf(stderr, \
                "ASSERTION FAILED: %s\nFile: %s\nLine: %d\nMessage: " fmt "\n", \
                #x, __FILE__, __LINE__, ##__VA_ARGS__); \
            DEBUG_BREAK(); \
        } \
    } while (0)
#else
#define ASSERT(x, msg) void(0)
#endif