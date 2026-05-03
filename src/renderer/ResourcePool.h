#ifndef GLDX_RESOURCE_POOL_H
#define GLDX_RESOURCE_POOL_H

#pragma once
#include <assert.h>
#include <stdint.h>

namespace CrossGLDX {

struct Handle {
  uint32_t idx;
  uint32_t generation;
};

template <typename ResourceType, uint32_t PoolSize> struct ResourcePool {

  struct Slot {
    ResourceType resource = {};
    uint32_t generation = 0;
    bool allocation_state = false;
  };

  Slot slots[PoolSize];
  uint32_t freeSlots[PoolSize];
  uint32_t freeSlotsCursor = 0;
  uint32_t count = 0;

  ResourcePool() {
    for (uint32_t i = 0; i < PoolSize; ++i) {
      freeSlots[i] = i;
    }
  }

  Handle Alloc() {
    assert(freeSlotsCursor < PoolSize && "ResourcePool overflow!");
    if (freeSlotsCursor >= PoolSize)
      return 0xFFFFFFFF;
    uint32_t idx = freeSlots[freeSlotsCursor++];
    slots[idx].allocation_state = true;
    count++;
    return {idx, slots[idx].generation};
  }

  void Free(uint32_t idx) {
    if (idx == 0xFFFFFFFF || idx >= PoolSize)
      return;
    assert(slots[idx].allocation_state && "Double free or invalid index!");
    slots[idx].resource = ResourceType{};
    slots[idx].generation++;
    slots[idx].allocation_state = false;
    freeSlots[--freeSlotsCursor] = idx;
    count--;
  }

  ResourceType &Get(uint32_t idx) {
    assert(idx < PoolSize && slots[idx].allocation_state);
    return slots[idx].resource;
  }

  const ResourceType &Get(uint32_t idx) const {
    assert(idx < PoolSize && slots[idx].allocation_state);
    return slots[idx].resource;
  }

  bool Validate(uint32_t idx, uint32_t generation) const {
    return idx < PoolSize && slots[idx].allocation_state &&
           slots[idx].generation == generation;
  }
};

} // namespace CrossGLDX

#endif
