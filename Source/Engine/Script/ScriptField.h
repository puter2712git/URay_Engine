#pragma once

#include <Core/Type/TypeAlias.h>

namespace URay
{

enum class ScriptFieldType : int32
{
    Unsupported = 0,
    Bool = 1,
    Int = 2,
    Float = 3,
    String = 4,
    Vector3 = 5
};

struct ScriptFieldInfo
{
    String typeName;
    String name;
    ScriptFieldType type = ScriptFieldType::Unsupported;
};

} // namespace URay
