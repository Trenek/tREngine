#include <assert.h>
#include <stdlib.h>

#include "resourceManager.h"

void addResource(struct ResourceManager *this, size_t id, void *mem, void (*cleanup)(void *)) {
    if (NULL == this->resources) {
        this->max = id + 1;
        this->resources = calloc(this->max, sizeof(struct resource));
    }
    else if (this->max <= id) {
        this->resources = realloc(this->resources, sizeof(struct resource) * (id + 1));
        for (size_t i = this->max; i < id + 1; i += 1) {
            this->resources[i].this = NULL;
            this->resources[i].cleanup = NULL;
        }
        this->max = id + 1;
    }

    this->resources[id].this = mem;
    this->resources[id].cleanup = cleanup;
}

void cleanupResource(struct ResourceManager *this, size_t id) {
    struct resource *res = &this->resources[id];
        
    if (NULL != res->this)
    if (NULL != res->cleanup) {
        res->cleanup(res->this);
    }

    res->this = NULL;
    res->cleanup = NULL;
}

void cleanupResourcesOrg(struct ResourceManager *this) {
    if (this->resources)
    for (size_t i = 0; i < this->max; i += 1) {
        cleanupResource(this, i);
    }

    free(this->resources);
}

void cleanupResourceManager(void *this) {
    cleanupResourcesOrg(this);

    free(this);
}

void *findResource(struct ResourceManager *this, size_t id) {
    assert(id < this->max);

    return this->resources[id].this;
}
