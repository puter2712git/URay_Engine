#pragma once

#include <Core/Type/TypeAlias.h>

namespace URay
{

struct Vector3;

struct TransformAPI
{
    int32(__cdecl* GetLocalPosition)(int32 index, uint32 generation, Vector3* outPosition);
    int32(__cdecl* SetLocalPosition)(int32 index, uint32 generation, const Vector3* position);
};

TransformAPI CreateTransformAPI();

} // namespace URay
