#pragma once

#include <Core/Math/Color.h>
#include <Core/Math/Vector2.h>
#include <Core/Math/Vector3.h>
#include <Core/Type/TypeAlias.h>
#include <Engine/Asset/AssetHandle.h>

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

using MaterialParameterValue = std::variant<float, Vector2, Vector3, Color, AssetHandle>;

struct MaterialParameter
{
    MaterialParameterType type;
    MaterialParameterValue value;
};

} // namespace URay
