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

struct MaterialParameterValue
{
    MaterialParameterType type;
    std::variant<float, Vector2, Vector3, Color, Texture*> value;
};

struct MaterialParameterDesc
{
    std::string name;
    MaterialParameterType type = MaterialParameterType::Float;
    uint32 set = 0;
    uint32 binding = 0;
};

} // namespace URay
