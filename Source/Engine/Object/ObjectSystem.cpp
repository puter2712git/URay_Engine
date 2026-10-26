#include "ObjectSystem.h"

#include <Engine/Object/Object.h>

namespace URay
{

Object* ObjectSystem::Resolve(ObjectId id) const
{
    if (id.index < 0 || static_cast<size_t>(id.index) >= slots.size())
        return nullptr;

    const ObjectSlot& slot = slots[id.index];

    if (slot.generation != id.generation)
        return nullptr;

    return slot.object;
}

void ObjectSystem::Register(Object& object)
{
    if (object.id.index != InvalidIndex)
        throw std::runtime_error("[ObjectSystem] Object is already registered.");

    int32 index;

    if (firstFree != InvalidIndex)
    {
        index = firstFree;

        ObjectSlot& slot = slots[index];
        firstFree = slot.nextFree;

        slot.object = &object;
        slot.nextFree = InvalidIndex;
    }
    else
    {
        if (slots.size() > static_cast<size_t>(std::numeric_limits<int32>::max()))
            throw std::length_error("[ObjectSystem] Maximum number of objects reached.");

        index = static_cast<int32>(slots.size());
        slots.push_back(ObjectSlot{ .object = &object });
    }

    object.id = ObjectId{
        .index = index,
        .generation = slots[index].generation
    };
}

bool ObjectSystem::Unregister(Object& object) noexcept
{
    const ObjectId id = object.id;

    if (id.index < 0 || static_cast<size_t>(id.index) >= slots.size())
        return false;

    ObjectSlot& slot = slots[id.index];

    if (slot.object != &object || slot.generation != id.generation)
        return false;

    slot.object = nullptr;
    object.id = {};

    if (slot.generation == std::numeric_limits<uint32>::max())
        return true;

    ++slot.generation;
    slot.nextFree = firstFree;
    firstFree = id.index;

    return true;
}

} // namespace URay
