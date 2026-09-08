#pragma once

#include <string>

namespace URay::Render
{

struct ShaderDefine
{
    std::wstring name;
    std::wstring value;

    bool operator==(const ShaderDefine&) const = default;
};

} // namespace URay::Render
