#include "InputBindings.h"

#include <Core/Input/InputEvent.h>
#include <Core/Input/InputSystem.h>
#include <Engine/Engine.h>

namespace URay::ScriptBindings
{

bool GetKey(ScriptSystem&, uint16 keyCode)
{
    return gEngine->GetInputSystem().GetKey(static_cast<KeyCode>(keyCode));
}

bool GetKeyDown(ScriptSystem&, uint16 keyCode)
{
    return gEngine->GetInputSystem().GetKeyDown(static_cast<KeyCode>(keyCode));
}

bool GetKeyUp(ScriptSystem&, uint16 keyCode)
{
    return gEngine->GetInputSystem().GetKeyUp(static_cast<KeyCode>(keyCode));
}

} // namespace URay::ScriptBindings
