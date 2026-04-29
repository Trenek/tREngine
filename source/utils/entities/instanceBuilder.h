#include <stddef.h>
#include <stdint.h>

struct instanceBuilder {
    size_t size;
    size_t bufferSize;
    void (*updater)(void *, void *, uint32_t, float);
};

