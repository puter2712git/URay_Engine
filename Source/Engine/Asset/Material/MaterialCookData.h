#pragma once

#include "Core/File/VirtualPath.h"
#include "Core/Math/Color.h"
#include "Core/Math/Vector2.h"
#include "Core/Math/Vector3.h"
#include "Core/Type/Types.h"
#include "Core/UUID.h"

#include <string>
#include <variant>
#include <vector>

namespace URay
{

enum class MaterialCookParameterType : uint8
{
    Float,
    Float2,
    Float3,
    Float4,
    Texture2D
};

struct MaterialCookParameter
{
    std::string name;
    MaterialCookParameterType type = MaterialCookParameterType::Float;

    std::variant<float, Vector2, Vector3, Color, VirtualPath> value;
};

struct MaterialCookData
{
    UUID shaderUUID = {};
    std::vector<MaterialCookParameter> parameters;
};

} // namespace URay
