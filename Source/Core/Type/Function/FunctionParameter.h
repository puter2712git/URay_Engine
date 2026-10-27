#pragma once

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
};

} // namespace URay
