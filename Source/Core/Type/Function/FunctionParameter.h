#pragma once

namespace URay
{

struct Type;

struct FunctionParameter
{
    const Type* type = nullptr;
    bool isConst = false;
    bool nullable = false;
};

} // namespace URay
