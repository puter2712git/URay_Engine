#pragma once

#include <string_view>

namespace URay
{

class ScriptSystem;

namespace ScriptBindings
{

bool Log(
    ScriptSystem& scriptSystem,
    std::string_view message);

} // namespace ScriptBindings

} // namespace URay
