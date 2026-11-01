#pragma once

#include <Core/Type/TypeAlias.h>
#include <Core/Type/Function/Callable/Callable.h>
#include <Core/Type/Function/FunctionSignature.h>
#include <Core/Type/Variant/Variant.h>

#include <cstddef>
#include <memory>
#include <span>
#include <utility>
#include <vector>

namespace URay
{

class Ray
{
public:
    explicit Ray(FunctionSignature signature);

public:
    bool Register(UniquePtr<Callable> callback);
    void UnregisterAll(const void* target);

    bool Emit(Span<const Variant> args);

    const FunctionSignature& GetSignature() const { return signature; }

private:
    struct Connection
    {
        explicit Connection(UniquePtr<Callable> callback) : callback(std::move(callback)) {}

        UniquePtr<Callable> callback;
        bool active = true;
    };

    FunctionSignature signature;
    Vector<std::shared_ptr<Connection>> callbacks;
};

} // namespace URay
