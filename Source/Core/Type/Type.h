#pragma once

#include <Core/Type/Types.h>

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
    uint32 size = 0;

    const Type* parent = nullptr;
};

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
        .name = "bool",
        .size = sizeof(bool)
    };
    return type;
}

template <>
inline const Type& TypeOf<int32>()
{
    static const Type type = {
        .kind = TypeKind::Int,
        .name = "int32",
        .size = sizeof(int32)
    };
    return type;
}

template <>
inline const Type& TypeOf<uint32>()
{
    static const Type type = {
        .kind = TypeKind::Int,
        .name = "uint32",
        .size = sizeof(uint32)
    };
    return type;
}

template <>
inline const Type& TypeOf<float>()
{
    static const Type type = {
        .kind = TypeKind::Float,
        .name = "float",
        .size = sizeof(float)
    };
    return type;
}

template <>
inline const Type& TypeOf<String>()
{
    static const Type type = {
        .kind = TypeKind::String,
        .name = "String",
        .size = sizeof(String)
    };
    return type;
}

#define URAY_TYPE_1(self)            \
public:                              \
    static const Type& StaticType()  \
    {                                \
        static const Type type = {   \
            .kind = TypeKind::Class, \
            .name = #self,           \
            .size = sizeof(self),    \
            .parent = nullptr        \
        };                           \
        return type;                 \
    }                                \
    const Type& GetType() const      \
    {                                \
        return self::StaticType();   \
    }

#define URAY_TYPE_2(self, parent)           \
public:                                     \
    static const Type& StaticType()         \
    {                                       \
        static const Type type = {          \
            .kind = TypeKind::Class,        \
            .name = #self,                  \
            .size = sizeof(self),           \
            .parent = &parent::StaticType() \
        };                                  \
        return type;                        \
    }                                       \
    const Type& GetType() const override    \
    {                                       \
        return self::StaticType();          \
    }

#define URAY_TYPE_SELECT(_1, _2, NAME, ...) NAME

#define URAY_TYPE(...) \
    URAY_TYPE_SELECT(__VA_ARGS__, URAY_TYPE_2, URAY_TYPE_1)(__VA_ARGS__)

} // namespace URay
