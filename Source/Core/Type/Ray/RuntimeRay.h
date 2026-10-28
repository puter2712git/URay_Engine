#pragma once

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

class RuntimeRay
{
public:
    explicit RuntimeRay(FunctionSignature signature);

public:
    bool Register(std::unique_ptr<Callable> callback);
    void UnregisterAll(const void* target);

    bool Emit(std::span<const Variant> args);

    const FunctionSignature& GetSignature() const { return signature; }

private:
    struct Connection
    {
        explicit Connection(std::unique_ptr<Callable> callback) : callback(std::move(callback)) {}

        std::unique_ptr<Callable> callback;
        bool active = true;
    };

    FunctionSignature signature;
    std::vector<std::shared_ptr<Connection>> callbacks;
};

} // namespace URay
