#pragma once

#include <Core/Math/Vector2.h>
#include <Core/Type/Types.h>

namespace URay
{

struct InputActionId
{
    uint32 value = 0;

    constexpr bool IsValid() const { return value != 0; }

    constexpr bool operator==(const InputActionId&) const = default;
};

enum class InputActionValueType : uint8
{
    Button,
    Axis1D,
    Axis2D
};

struct InputActionValue
{
    bool held = false;
    bool pressed = false;
    bool released = false;

    float axis1D = 0.0f;
    Vector2 axis2D = Vector2::Zero;
};

} // namespace URay
