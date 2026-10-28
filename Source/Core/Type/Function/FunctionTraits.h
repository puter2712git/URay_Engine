#pragma once

#include <cstddef>
#include <tuple>

namespace URay
{

template <typename T>
struct FunctionTraits;

template <typename Return, typename... Args>
struct FunctionTraits<Return (*)(Args...)>
{
    using OwnerType = void;
    using ReturnType = Return;
    using ArgumentTypes = std::tuple<Args...>;

    static constexpr std::size_t ArgumentCount = sizeof...(Args);
    static constexpr bool IsMember = false;
    static constexpr bool IsConst = false;
};

// Member function
template <typename Owner, typename Return, typename... Args>
struct FunctionTraits<Return (Owner::*)(Args...)>
{
    using OwnerType = Owner;
    using ReturnType = Return;
    using ArgumentTypes = std::tuple<Args...>;

    static constexpr std::size_t ArgumentCount = sizeof...(Args);
    static constexpr bool IsMember = true;
    static constexpr bool IsConst = false;
};

// Const member function
template <typename Owner, typename Return, typename... Args>
struct FunctionTraits<Return (Owner::*)(Args...) const>
{
    using OwnerType = Owner;
    using ReturnType = Return;
    using ArgumentTypes = std::tuple<Args...>;

    static constexpr std::size_t ArgumentCount = sizeof...(Args);
    static constexpr bool IsMember = true;
    static constexpr bool IsConst = true;
};

} // namespace URay
