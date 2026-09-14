#pragma once

#include "Core/Math/Color.h"
#include "Core/Math/Vector2.h"
#include "Core/Math/Vector3.h"
#include "Core/Type/Types.h"

#include <string>
#include <variant>

namespace URay
{

class Texture;

enum class MaterialParameterType
{
    Float,
    Float2,
    Float3,
    Float4,
    Texture2D
};

struct MaterialParameter
{
    uint32 binding = 0;

    MaterialParameterType type;
    std::variant<float, Vector2, Vector3, Color, Texture*> value;

    uint32 offset = 0;
    uint32 size = 0;
};

} // namespace URay
