#include "TransformAPI.h"

#include <Core/Math/Vector3.h>
#include <Core/Type/TypeAlias.h>
#include <Engine/Component/TransformComponent.h>
#include <Engine/Engine.h>
#include <Engine/Object/ObjectId.h>
#include <Engine/Object/ObjectSystem.h>

#include <cstddef>
#include <cstring>
#include <type_traits>

namespace URay
{

static_assert(sizeof(float) == 4);

static_assert(std::is_standard_layout_v<Vector3>);
static_assert(std::is_trivially_copyable_v<Vector3>);

static_assert(sizeof(Vector3) == 12);
static_assert(alignof(Vector3) == 4);
static_assert(offsetof(Vector3, x) == 0);
static_assert(offsetof(Vector3, y) == 4);
static_assert(offsetof(Vector3, z) == 8);

namespace
{

TransformComponent* ResolveTransform(int32 index, uint32 generation)
{
    const ObjectId id{
        .index = index,
        .generation = generation
    };

    Object* object = gEngine->GetObjectSystem().Resolve(id);
    return Cast<TransformComponent>(object);
}

int32 __cdecl GetLocalPosition(int32 index, uint32 generation, Vector3* outPosition) noexcept
{
    if (!outPosition)
        return -1;

    try
    {
        TransformComponent* transform = ResolveTransform(index, generation);
        if (!transform)
            return 0;

        const Vector3& position = transform->GetLocalPosition();

        std::memcpy(outPosition, &position, sizeof(position));
        return 1;
    }
    catch (...)
    {
        return -1;
    }
}

int32 __cdecl SetLocalPosition(int32 index, uint32 generation, const Vector3* position) noexcept
{
    if (!position)
        return -1;

    try
    {
        TransformComponent* transform = ResolveTransform(index, generation);
        if (!transform)
            return 0;

        Vector3 value;
        std::memcpy(&value, position, sizeof(value));

        transform->SetLocalPosition(value);
        return 1;
    }
    catch (...)
    {
        return -1;
    }
}

} // namespace

TransformAPI CreateTransformAPI()
{
    return TransformAPI{
        .GetLocalPosition = GetLocalPosition,
        .SetLocalPosition = SetLocalPosition
    };
}

} // namespace URay
