#pragma once

#include <Core/Math/Color.h>
#include <Core/Math/Quaternion.h>
#include <Core/Math/Vector2.h>
#include <Core/Math/Vector3.h>
#include <Core/Math/Vector4.h>
#include <Core/Type/TypeAlias.h>

namespace URay
{

enum class TypeKind
{
    Null,
    Bool,
    Int,
    Float,
    String,

    Vector2,
    Vector3,
    Vector4,
    Quaternion,
    Color,

    Class,
    Array,
    Dictionary
};

struct Type
{
    TypeKind kind = TypeKind::Null;

    String name;
    const Type* parent = nullptr;
};

inline bool IsTypeOf(const Type& derived, const Type& target)
{
    for (const Type* type = &derived; type; type = type->parent)
    {
        if (type == &target)
            return true;
    }

    return false;
}

template <typename To, typename From>
To* Cast(From* source)
{
    if (!source)
        return nullptr;

    if (!IsTypeOf(source->GetType(), To::StaticType()))
        return nullptr;

    return static_cast<To*>(source);
}

template <typename T>
inline const Type& TypeOf()
{
    return T::StaticType();
}

template <>
inline const Type& TypeOf<bool>()
{
    static const Type type = {
        .kind = TypeKind::Bool,
        .name = "bool"
    };
    return type;
}

template <>
inline const Type& TypeOf<int32>()
{
    static const Type type = {
        .kind = TypeKind::Int,
        .name = "int32"
    };
    return type;
}

template <>
inline const Type& TypeOf<uint32>()
{
    static const Type type = {
        .kind = TypeKind::Int,
        .name = "uint32"
    };
    return type;
}

template <>
inline const Type& TypeOf<float>()
{
    static const Type type = {
        .kind = TypeKind::Float,
        .name = "float"
    };
    return type;
}

template <>
inline const Type& TypeOf<String>()
{
    static const Type type = {
        .kind = TypeKind::String,
        .name = "String"
    };
    return type;
}

template <>
inline const Type& TypeOf<Vector2>()
{
    static const Type type = {
        .kind = TypeKind::Vector2,
        .name = "Vector2"
    };
    return type;
}

template <>
inline const Type& TypeOf<Vector3>()
{
    static const Type type = {
        .kind = TypeKind::Vector3,
        .name = "Vector3"
    };
    return type;
}

template <>
inline const Type& TypeOf<Vector4>()
{
    static const Type type = {
        .kind = TypeKind::Vector4,
        .name = "Vector4"
    };
    return type;
}

template <>
inline const Type& TypeOf<Quaternion>()
{
    static const Type type = {
        .kind = TypeKind::Quaternion,
        .name = "Quaternion"
    };
    return type;
}

template <>
inline const Type& TypeOf<Color>()
{
    static const Type type = {
        .kind = TypeKind::Color,
        .name = "Color"
    };
    return type;
}

#define URAY_TYPE_1(self)               \
public:                                 \
    static const Type& StaticType()     \
    {                                   \
        static const Type type = {      \
            .kind = TypeKind::Class,    \
            .name = #self,              \
            .parent = nullptr           \
        };                              \
        return type;                    \
    }                                   \
    virtual const Type& GetType() const \
    {                                   \
        return self::StaticType();      \
    }

#define URAY_TYPE_2(self, parentType)           \
    using Super = parentType;                   \
                                                \
public:                                         \
    static const Type& StaticType()             \
    {                                           \
        static const Type type = {              \
            .kind = TypeKind::Class,            \
            .name = #self,                      \
            .parent = &parentType::StaticType() \
        };                                      \
        return type;                            \
    }                                           \
    const Type& GetType() const override        \
    {                                           \
        return self::StaticType();              \
    }

#define URAY_TYPE_SELECT(_1, _2, NAME, ...) NAME

#define URAY_TYPE(...) \
    URAY_TYPE_SELECT(__VA_ARGS__, URAY_TYPE_2, URAY_TYPE_1)(__VA_ARGS__)

} // namespace URay
