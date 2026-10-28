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
    bool Emit(std::span<const Variant> args);

    const FunctionSignature& GetSignature() const { return signature; }

private:
    FunctionSignature signature;
    std::vector<std::unique_ptr<Callable>> callbacks;
};

} // namespace URay
