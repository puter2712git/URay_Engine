#include "InputBindings.h"

#include <Core/Input/InputEvent.h>
#include <Core/Input/InputSystem.h>
#include <Engine/Engine.h>

namespace URay::ScriptBindings
{

bool GetKey(ScriptSystem&, uint16 keyCode)
{
    return gEngine->GetInputSystem().GetSnapshot().IsKeyHeld(static_cast<KeyCode>(keyCode));
}

bool GetKeyDown(ScriptSystem&, uint16 keyCode)
{
    return gEngine->GetInputSystem().GetSnapshot().WasKeyPressed(static_cast<KeyCode>(keyCode));
}

bool GetKeyUp(ScriptSystem&, uint16 keyCode)
{
    return gEngine->GetInputSystem().GetSnapshot().WasKeyReleased(static_cast<KeyCode>(keyCode));
}

} // namespace URay::ScriptBindings
