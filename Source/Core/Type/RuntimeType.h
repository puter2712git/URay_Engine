#pragma once

#include <string_view>

namespace URay
{

struct RuntimeType
{
    std::string_view name;
    const RuntimeType* parent;
};

#define URAY_ROOT_TYPE(self)                          \
public:                                               \
    static const RuntimeType& StaticRuntimeType()     \
    {                                                 \
        static const RuntimeType type{                \
            .name = #self,                            \
            .parent = nullptr                         \
        };                                            \
        return type;                                  \
    }                                                 \
                                                      \
    virtual const RuntimeType& GetRuntimeType() const \
    {                                                 \
        return self::StaticRuntimeType();             \
    }

#define URAY_TYPE(self, parentType)                    \
public:                                                \
    using Super = parentType;                          \
                                                       \
    static const RuntimeType& StaticRuntimeType()      \
    {                                                  \
        static const RuntimeType type{                 \
            .name = #self,                             \
            .parent = &parentType::StaticRuntimeType() \
        };                                             \
        return type;                                   \
    }                                                  \
                                                       \
    const RuntimeType& GetRuntimeType() const override \
    {                                                  \
        return self::StaticRuntimeType();              \
    }

inline bool IsTypeOf(const RuntimeType& derived, const RuntimeType& target)
{
    for (const RuntimeType* type = &derived; type; type = type->parent)
    {
        if (type == &target)
            return true;
    }

    return false;
}

} // namespace URay
