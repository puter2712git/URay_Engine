#pragma once

#include <Core/Math/Vector3.h>
#include <Core/Type/Types.h>

namespace URay
{

class ScriptSystem;

namespace ScriptBindings
{

bool GetTransformPosition(
    ScriptSystem& scriptSystem,
    uint64 instanceId,
    Vector3& position);

bool SetTransformPosition(
    ScriptSystem& scriptSystem,
    uint64 instanceId,
    const Vector3& position);

} // namespace ScriptBindings

} // namespace URay