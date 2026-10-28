#pragma once

#include <Core/Type/Function/Callable/MemberCallable.h>
#include <Core/Type/Function/FunctionSignature.h>
#include <Core/Type/Ray/RuntimeRay.h>
#include <Core/Type/TypeAlias.h>
#include <Core/Type/Variant/Variant.h>

#include <array>
#include <unordered_map>
#include <utility>

namespace URay
{

class RayEmitter
{
public:
    template <typename... Args>
    bool DeclareRay(const String& name)
    {
        if (rays.contains(name))
            return false;

        using SignatureFunction = void (*)(Args...);

        auto signature = MakeFunctionSignature(static_cast<SignatureFunction>(nullptr));

        if (!signature)
            return false;

        rays.try_emplace(name, std::move(*signature));
        return true;
    }

    template <typename Target, typename Method>
    bool Register(const String& name, Target* target, Method method)
    {
        auto it = rays.find(name);

        if (it == rays.end())
            return false;

        auto callback = MakeMemberCallable(target, method);

        if (!callback)
            return false;

        return it->second.Register(std::move(callback));
    }

    template <typename... Args>
    bool Emit(const String& name, Args&&... args)
    {
        auto it = rays.find(name);

        if (it == rays.end())
            return false;

        std::array<Variant, sizeof...(Args)> arguments{
            Variant(std::forward<Args>(args))...
        };

        return it->second.Emit(arguments);
    }

private:
    std::unordered_map<String, RuntimeRay> rays;
};

#define URAY_DECLARE_RAY(name, ...) \
    DeclareRay<__VA_ARGS__>(#name);

} // namespace URay
