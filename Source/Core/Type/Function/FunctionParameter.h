#pragma once

#include <Core/Type/Function/FunctionTraits.h>
#include <Core/Type/Type.h>

#include <optional>
#include <type_traits>

namespace URay
{

struct Type;
class Variant;

struct FunctionParameter
{
    const Type* type = nullptr;
    bool isConst = false;
    bool nullable = false;

    bool Accepts(const Variant& argument) const;
    bool Accepts(const FunctionParameter& source) const;
};

template <typename T>
std::optional<FunctionParameter> MakeFunctionParameter()
{
    static_assert(
        !std::is_reference_v<T>,
        "Function parameters must not be references. Use pointers or const pointers instead.");

    using RawType = std::remove_cv_t<T>;

    if constexpr (std::is_pointer_v<RawType>)
    {
        using Pointee = std::remove_pointer_t<RawType>;
        using ValueType = std::remove_cv_t<Pointee>;

        static_assert(
            !std::is_pointer_v<ValueType>,
            "Pointers to pointers are not supported.");

        static_assert(
            !std::is_volatile_v<Pointee>,
            "Volatile pointers are not supported.");

        const Type& type = TypeOf<ValueType>();

        if (type.kind != TypeKind::Class)
            return std::nullopt;

        if (type.isObjectType && std::is_const_v<Pointee>)
            return std::nullopt;

        return FunctionParameter{
            .type = &type,
            .isConst = std::is_const_v<Pointee>,
            .nullable = false
        };
    }
    else
    {
        const Type& type = TypeOf<RawType>();

        if (type.kind != TypeKind::Value)
            return std::nullopt;

        return FunctionParameter{
            .type = &type,
            .isConst = false,
            .nullable = false
        };
    }
}

} // namespace URay
