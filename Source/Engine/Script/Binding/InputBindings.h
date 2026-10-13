#pragma once

#include <Core/Type/Types.h>

namespace URay
{

class ScriptSystem;

namespace ScriptBindings
{

bool GetKey(ScriptSystem& scriptSystem, uint16 keyCode);
bool GetKeyDown(ScriptSystem& scriptSystem, uint16 keyCode);
bool GetKeyUp(ScriptSystem& scriptSystem, uint16 keyCode);

} // namespace ScriptBindings

} // namespace URay
