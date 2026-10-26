#pragma once

#include <Core/Type/TypeAlias.h>
#include <Engine/Object/ObjectId.h>

#include <vector>

namespace URay
{

class Object;

struct ObjectSlot
{
    Object* object = nullptr;
    uint32 generation = 1;
    int32 nextFree = InvalidIndex;
};

class ObjectSystem
{
    friend class Object;

public:
    Object* Resolve(ObjectId id) const;

private:
    void Register(Object& object);
    bool Unregister(Object& object) noexcept;

private:
    std::vector<ObjectSlot> slots;
    int32 firstFree = InvalidIndex;
};

} // namespace URay
