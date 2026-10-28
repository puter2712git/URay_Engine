#pragma once

#include <Core/Type/TypeAlias.h>

namespace URay
{

enum class TypeKind
{
    Null,
    Value,
    Class
};

struct Type
{
    TypeKind kind = TypeKind::Null;

    String name;
    const Type* parent = nullptr;

    bool isObjectType = false;
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
        .kind = TypeKind::Value,
        .name = "bool"
    };
    return type;
}

template <>
inline const Type& TypeOf<int32>()
{
    static const Type type = {
        .kind = TypeKind::Value,
        .name = "int32"
    };
    return type;
}

template <>
inline const Type& TypeOf<uint32>()
{
    static const Type type = {
        .kind = TypeKind::Value,
        .name = "uint32"
    };
    return type;
}

template <>
inline const Type& TypeOf<float>()
{
    static const Type type = {
        .kind = TypeKind::Value,
        .name = "float"
    };
    return type;
}

template <>
inline const Type& TypeOf<String>()
{
    static const Type type = {
        .kind = TypeKind::Value,
        .name = "String"
    };
    return type;
}

template <>
inline const Type& TypeOf<char32_t>()
{
    static const Type type = {
        .kind = TypeKind::Value,
        .name = "char32_t"
    };
    return type;
}

#define URAY_ROOT_TYPE(self, objectType) \
public:                                  \
    static const Type& StaticType()      \
    {                                    \
        static const Type type = {       \
            .kind = TypeKind::Class,     \
            .name = #self,               \
            .parent = nullptr,           \
            .isObjectType = objectType   \
        };                               \
        return type;                     \
    }                                    \
    virtual const Type& GetType() const  \
    {                                    \
        return self::StaticType();       \
    }

#define URAY_CLASS_1(self) URAY_ROOT_TYPE(self, false)

#define URAY_OBJECT_ROOT_TYPE(self) URAY_ROOT_TYPE(self, true)

#define URAY_CLASS_2(self, parentType)                            \
    using Super = parentType;                                     \
                                                                  \
public:                                                           \
    static const Type& StaticType()                               \
    {                                                             \
        static const Type type = {                                \
            .kind = TypeKind::Class,                              \
            .name = #self,                                        \
            .parent = &parentType::StaticType(),                  \
            .isObjectType = parentType::StaticType().isObjectType \
        };                                                        \
        return type;                                              \
    }                                                             \
    const Type& GetType() const override                          \
    {                                                             \
        return self::StaticType();                                \
    }

#define URAY_CLASS_SELECT(_1, _2, NAME, ...) NAME

#define URAY_CLASS(...) \
    URAY_CLASS_SELECT(__VA_ARGS__, URAY_CLASS_2, URAY_CLASS_1)(__VA_ARGS__)

// Registers a non-polymorphic value type without changing its object layout.
#define URAY_TYPE(self)              \
public:                              \
    static const Type& StaticType()  \
    {                                \
        static const Type type = {   \
            .kind = TypeKind::Value, \
            .name = #self            \
        };                           \
        return type;                 \
    }

#define URAY_PLAIN_CLASS(self)       \
public:                              \
    static const Type& StaticType()  \
    {                                \
        static const Type type = {   \
            .kind = TypeKind::Class, \
            .name = #self,           \
            .parent = nullptr,       \
            .isObjectType = false    \
        };                           \
        return type;                 \
    }

#define URAY_ENUM_TYPE(enumType)          \
    template <>                           \
    inline const Type& TypeOf<enumType>() \
    {                                     \
        static const Type type = {        \
            .kind = TypeKind::Value,      \
            .name = #enumType             \
        };                                \
        return type;                      \
    }

} // namespace URay
