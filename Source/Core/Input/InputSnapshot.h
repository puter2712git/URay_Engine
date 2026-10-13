#pragma once

#include <Core/Input/InputEvent.h>
#include <Core/Math/Vector2.h>
#include <Core/Type/Types.h>

#include <array>
#include <optional>
#include <vector>

namespace URay
{

struct InputButtonState
{
    bool held = false;
    bool pressed = false;
    bool released = false;
};

struct InputSnapshot
{
    friend class InputSystem;

public:
    uint64 GetFrameIndex() const { return frameIndex; }

    bool IsKeyHeld(KeyCode key) const;
    bool WasKeyPressed(KeyCode key) const;
    bool WasKeyReleased(KeyCode key) const;

    bool IsMouseButtonHeld(MouseButton button) const;
    bool WasMouseButtonPressed(MouseButton button) const;
    bool WasMouseButtonReleased(MouseButton button) const;

    const Vector2& GetMousePosition() const { return mousePosition; }
    const Vector2& GetMouseDelta() const { return mouseDelta; }
    const Vector2& GetScrollDelta() const { return scrollDelta; }
    const std::vector<InputEvent>& GetEvents() const { return events; }

    bool IsWindowFocused() const { return windowFocused; }

    ModifierKey GetHeldModifiers() const;

private:
    static std::optional<size_t> GetMouseButtonIndex(MouseButton button);
    static MouseButton GetMouseButtonByIndex(size_t index);

    const InputButtonState* GetKeyState(KeyCode key) const;
    const InputButtonState* GetMouseButtonState(MouseButton button) const;

private:
    uint64 frameIndex = 0;

    std::array<InputButtonState, static_cast<size_t>(KeyCode::Count)> keys = {};
    std::array<InputButtonState, 3> mouseButtons = {};

    Vector2 mousePosition = Vector2::Zero;
    Vector2 mouseDelta = Vector2::Zero;
    Vector2 scrollDelta = Vector2::Zero;

    std::vector<InputEvent> events;

    bool windowFocused = true;
};

} // namespace URay
