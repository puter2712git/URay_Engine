#include "ObjectAPI.h"

#include <Core/Type/TypeAlias.h>
#include <Engine/Engine.h>
#include <Engine/Object/ObjectId.h>
#include <Engine/Object/ObjectSystem.h>

namespace URay
{

namespace
{

int32 __cdecl IsObjectValid(int32 index, uint32 generation) noexcept
{
    try
    {
        const ObjectId id{
            .index = index,
            .generation = generation
        };

        return gEngine->GetObjectSystem().Resolve(id) ? 1 : 0;
    }
    catch (...)
    {
        return -1;
    }
}

} // namespace

ObjectAPI CreateObjectAPI()
{
    return ObjectAPI{
        .IsValid = &IsObjectValid
    };
}

} // namespace URay
