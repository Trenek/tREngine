#include <assert.h>

extern int printf (const char *__restrict __format, ...);
[[noreturn]] extern void exit (int __status);

#ifdef NDEBUG
    #define MY_ASSERT(x) do if (!(x)) { printf("Error: (%s)\n", #x); exit(0); } while (0)
#else
    #define MY_ASSERT(x) assert(x)
#endif
