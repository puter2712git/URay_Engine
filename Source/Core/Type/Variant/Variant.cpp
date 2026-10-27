#include "Variant.h"

#include <Engine/Engine.h>
#include <Engine/Object/Object.h>
#include <Engine/Object/ObjectId.h>
#include <Engine/Object/ObjectSystem.h>

namespace URay
{

Variant::Variant(Object* object)
{
    if (!object)
        return;

    type = &object->GetType();
    ops = &ValueVariantPolicy<ObjectId>::Operations();
    value = new ObjectId(object->GetId());
}

bool Variant::IsConstPointer() const noexcept
{
    return ops && ops->isConstPointer;
}

bool Variant::IsNullPointer() const noexcept
{
    return ops && ops->isNullPointer && ops->isNullPointer(value);
}

void Variant::Reset() noexcept
{
    if (ops)
        ops->destroy(value);

    type = nullptr;
    ops = nullptr;
    value = nullptr;
}

Object* Variant::ResolveObject() const
{
    if (!type || !type->isObjectType)
        return nullptr;

    const ObjectId& id = *static_cast<const ObjectId*>(value);

    ObjectSystem& objectSystem = gEngine->GetObjectSystem();
    return objectSystem.Resolve(id);
}

} // namespace URay
