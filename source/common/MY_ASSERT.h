#include <assert.h>

#ifdef NDEBUG
    #define MY_ASSERT(x) if (!(x)) exit(0);
#else
    #define MY_ASSERT(x) assert(x)
#endif
