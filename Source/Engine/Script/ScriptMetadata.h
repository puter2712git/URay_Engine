#pragma once

#include <Core/Math/Color.h>
#include <Core/Math/Vector2.h>
#include <Core/Math/Vector3.h>
#include <Core/Math/Vector4.h>
#include <Core/Type/Types.h>
#include <Engine/Object/Property/PropertyTypes.h>

#include <string>
#include <variant>
#include <vector>

namespace URay
{

using ScriptValue = std::variant<
    bool,
    int32,
    float,
    Vector2,
    Vector3,
    Vector4,
    Color,
    std::string>;

struct ScriptFieldDefinition
{
    std::string name;
    PropertyType type = PropertyType::Bool;
    ScriptValue defaultValue = false;
};

struct ScriptTypeDefinition
{
    std::string className;
    std::vector<ScriptFieldDefinition> fields;
};

} // namespace URay
