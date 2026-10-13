#include "InputSystem.h"

namespace URay
{

void InputSystem::BeginFrame()
{
    pendingSnapshot = publishedSnapshot;

    for (InputButtonState& state : pendingSnapshot.keys)
    {
        state.pressed = false;
        state.released = false;
    }

    for (InputButtonState& state : pendingSnapshot.mouseButtons)
    {
        state.pressed = false;
        state.released = false;
    }

    pendingSnapshot.mouseDelta = Vector2::Zero;
    pendingSnapshot.scrollDelta = Vector2::Zero;
    pendingSnapshot.events.clear();
}

void InputSystem::PublishSnapshot(uint64 frameIndex)
{
    pendingSnapshot.frameIndex = frameIndex;
    publishedSnapshot = pendingSnapshot;
}

void InputSystem::OnKey(KeyCode key, KeyAction action, ModifierKey modifiers)
{
    const size_t keyIndex = static_cast<size_t>(key);
    if (keyIndex >= static_cast<size_t>(KeyCode::Count))
        return;

    InputButtonState& state = pendingSnapshot.keys[keyIndex];

    switch (action)
    {
    case KeyAction::Pressed:
        if (!state.held)
            state.pressed = true;
        state.held = true;
        break;

    case KeyAction::Repeated:
        state.held = true;
        break;

    case KeyAction::Released:
        if (state.held)
            state.released = true;
        state.held = false;
        break;
    }

    pendingSnapshot.events.push_back(KeyEvent{
        .key = key,
        .modifiers = modifiers,
        .action = action,
    });
}

void InputSystem::OnMouseButton(MouseButton button, KeyAction action, ModifierKey modifiers)
{
    const std::optional<size_t> buttonIndex = InputSnapshot::GetMouseButtonIndex(button);
    if (!buttonIndex)
        return;

    InputButtonState& state = pendingSnapshot.mouseButtons[*buttonIndex];

    switch (action)
    {
    case KeyAction::Pressed:
        if (!state.held)
            state.pressed = true;
        state.held = true;
        break;

    case KeyAction::Repeated:
        state.held = true;
        break;

    case KeyAction::Released:
        if (state.held)
            state.released = true;
        state.held = false;
        break;
    }

    pendingSnapshot.events.push_back(PointerEvent{
        .action = action == KeyAction::Released
                      ? PointerAction::Released
                      : PointerAction::Pressed,
        .position = pendingSnapshot.mousePosition,
        .delta = Vector2::Zero,
        .changedButton = button,
        .pressedButtons = BuildPressedMouseButtonMask(),
        .modifiers = modifiers,
    });
}

void InputSystem::OnCursorMoved(Vector2 position)
{
    const Vector2 delta = position - pendingSnapshot.mousePosition;

    pendingSnapshot.mouseDelta += delta;
    pendingSnapshot.mousePosition = position;

    pendingSnapshot.events.push_back(PointerEvent{
        .action = PointerAction::Moved,
        .position = position,
        .delta = delta,
        .changedButton = MouseButton::None,
        .pressedButtons = BuildPressedMouseButtonMask(),
        .modifiers = pendingSnapshot.GetHeldModifiers() });
}

void InputSystem::OnTextInput(char32_t codepoint)
{
    pendingSnapshot.events.push_back(TextInputEvent{
        .codepoint = codepoint });
}

void InputSystem::OnScroll(Vector2 delta)
{
    pendingSnapshot.scrollDelta += delta;

    pendingSnapshot.events.push_back(ScrollEvent{
        .position = pendingSnapshot.mousePosition,
        .delta = delta,
        .modifiers = pendingSnapshot.GetHeldModifiers() });
}

void InputSystem::OnWindowFocusChanged(bool focused)
{
    pendingSnapshot.windowFocused = focused;

    if (!focused)
    {
        pendingSnapshot.mouseDelta = Vector2::Zero;
        pendingSnapshot.scrollDelta = Vector2::Zero;
    }

    pendingSnapshot.events.push_back(WindowFocusEvent{
        .focused = focused });
}

MouseButtonMask InputSystem::BuildPressedMouseButtonMask() const
{
    MouseButtonMask mask = 0;

    for (size_t index = 0; index < pendingSnapshot.mouseButtons.size(); ++index)
    {
        if (pendingSnapshot.mouseButtons[index].held)
            mask |= static_cast<MouseButtonMask>(InputSnapshot::GetMouseButtonByIndex(index));
    }

    return mask;
}

} // namespace URay
