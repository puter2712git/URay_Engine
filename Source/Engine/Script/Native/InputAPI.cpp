#include "InputAPI.h"

#include <Core/Input/InputEvent.h>
#include <Core/Input/InputSystem.h>
#include <Engine/Engine.h>

namespace URay
{

namespace
{

int32 __cdecl IsKeyHeld(int32 key) noexcept
{
    if (key <= static_cast<int32>(KeyCode::Unknown) || key >= static_cast<int32>(KeyCode::Count))
        return -1;

    try
    {
        const InputSnapshot& snapshot = gEngine->GetInputSystem().GetSnapshot();
        return snapshot.IsKeyHeld(static_cast<KeyCode>(key)) ? 1 : 0;
    }
    catch (...)
    {
        return -1;
    }
}

} // namespace

InputAPI CreateInputAPI()
{
    return InputAPI{
        .IsKeyHeld = &IsKeyHeld
    };
}

} // namespace URay
