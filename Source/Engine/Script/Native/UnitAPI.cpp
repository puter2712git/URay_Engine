#include "UnitAPI.h"

#include <Engine/Component/TransformComponent.h>
#include <Engine/Engine.h>
#include <Engine/Object/ObjectId.h>
#include <Engine/Object/ObjectSystem.h>
#include <Engine/Scene/Unit.h>

namespace URay
{

namespace
{

int32 __cdecl GetUnitTransform(
    int32 unitIndex,
    uint32 unitGeneration,
    int32* outIndex,
    uint32* outGeneration) noexcept
{
    if (!outIndex || !outGeneration)
        return -1;

    *outIndex = InvalidIndex;
    *outGeneration = 0;

    try
    {
        const ObjectId unitId{
            .index = unitIndex,
            .generation = unitGeneration
        };

        Object* object = gEngine->GetObjectSystem().Resolve(unitId);
        Unit* unit = Cast<Unit>(object);
        if (!unit)
            return 0;

        TransformComponent* transform = unit->GetTransform();
        if (!transform)
            return 0;

        const ObjectId transformId = transform->GetId();
        *outIndex = transformId.index;
        *outGeneration = transformId.generation;

        return 1;
    }
    catch (...)
    {
        return -1;
    }
}

} // namespace

UnitAPI CreateUnitAPI()
{
    return UnitAPI{
        .GetTransform = GetUnitTransform
    };
}

} // namespace URay
