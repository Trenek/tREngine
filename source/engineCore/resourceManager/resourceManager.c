#include <assert.h>
#include <stdlib.h>

#include "resourceManager.h"

void addResource(struct ResourceManager *this, size_t id, void *mem, void (*cleanup)(void *)) {
    if (NULL == this->resources) {
        this->max = id + 1;
        this->resources = malloc(sizeof(struct resource) * this->max);
    }
    else if (this->max <= id) {
        this->max = id + 1;
        this->resources = realloc(this->resources, sizeof(struct resource) * this->max);
    }

    this->resources[id].this = mem;
    this->resources[id].cleanup = cleanup;
}

void cleanupResource(struct ResourceManager *this, size_t id) {
    if (this->resources[id].this)
    if (this->resources[id].cleanup) {
        this->resources[id].cleanup(this->resources[id].this);
    }
}

void cleanupResourcesOrg(struct ResourceManager *this) {
    for (size_t i = 0; i < this->max; i += 1) {
        if (this->resources)
        if (this->resources[i].cleanup) {
            this->resources[i].cleanup(this->resources[i].this);
        }
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
