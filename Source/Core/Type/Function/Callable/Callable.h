#pragma once

#include <Core/Type/Function/FunctionSignature.h>

#include <span>
#include <utility>

namespace URay
{

class Variant;

class Callable
{
public:
    explicit Callable(FunctionSignature signature)
        : signature(std::move(signature)) {}

    virtual ~Callable() = default;

    Callable(const Callable&) = delete;
    Callable& operator=(const Callable&) = delete;

public:
    const FunctionSignature& GetSignature() const { return signature; }

    virtual bool Invoke(std::span<const Variant> args) = 0;
    virtual bool HasTarget(const void* target) const = 0;

private:
    FunctionSignature signature;
};

} // namespace URay
