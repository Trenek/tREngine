#include <stddef.h>

struct resource {
    void *this;
    void (*cleanup)(void *);
};

struct ResourceManager {
    size_t max;
    struct resource *resources;
};

void addResource(struct ResourceManager *this, size_t id, void *mem, void (*cleanup)(void *));
void cleanupResource(struct ResourceManager *this, size_t id);
void cleanupResourcesOrg(struct ResourceManager *this);
void cleanupResourceManager(void *this);

void *findResource(struct ResourceManager *this, size_t id);
