#pragma once

#include <Core/Math/Color.h>
#include <Core/Math/Quaternion.h>
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

enum class ScriptFieldType : int32
{
    Bool = 0,
    Int = 1,
    Float = 2,
    Vector2 = 3,
    Vector3 = 4,
    Vector4 = 5,
    Color = 6,
    String = 7
};

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

struct ScriptFieldValue
{
    std::string name;
    PropertyType type = PropertyType::Bool;
    ScriptValue value = false;
};

enum class ScriptLifecycle : int32
{
    Initialize = 0,
    Load = 1,
    Update = 2,
    Unload = 3,
    Finalize = 4
};

} // namespace URay
