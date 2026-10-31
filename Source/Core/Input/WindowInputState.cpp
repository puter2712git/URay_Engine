#include "WindowInputState.h"

namespace URay
{

void WindowInputState::BeginFrame()
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

void WindowInputState::PublishSnapshot(uint64 frameIndex)
{
    pendingSnapshot.frameIndex = frameIndex;
    publishedSnapshot = pendingSnapshot;
}

void WindowInputState::OnKey(KeyCode key, KeyAction action, ModifierKey modifiers)
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

void WindowInputState::OnMouseButton(MouseButton button, KeyAction action, ModifierKey modifiers)
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

    uint8 clickCount = 0;

    if (action == KeyAction::Pressed)
    {
        constexpr auto multiClickInterval = std::chrono::milliseconds(500);
        constexpr float multiClickMaxDistance = 4.0f;

        const auto currentTime = std::chrono::steady_clock::now();

        const float deltaX = pendingSnapshot.mousePosition.x - lastClickPosition.x;
        const float deltaY = pendingSnapshot.mousePosition.y - lastClickPosition.y;

        const bool isSameClickSequence = button == lastClickButton &&
                                         currentTime - lastClickTime <= multiClickInterval &&
                                         deltaX * deltaX + deltaY * deltaY <= multiClickMaxDistance * multiClickMaxDistance;

        if (isSameClickSequence)
            ++consecutiveClickCount;
        else
            consecutiveClickCount = 1;

        lastClickTime = currentTime;
        lastClickPosition = pendingSnapshot.mousePosition;
        lastClickButton = button;

        clickCount = consecutiveClickCount;
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
        .clickCount = clickCount });
}

void WindowInputState::OnCursorMoved(Vector2 position)
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

void WindowInputState::OnTextInput(char32_t codepoint)
{
    pendingSnapshot.events.push_back(TextInputEvent{
        .codepoint = codepoint });
}

void WindowInputState::OnScroll(Vector2 delta)
{
    pendingSnapshot.scrollDelta += delta;

    pendingSnapshot.events.push_back(ScrollEvent{
        .position = pendingSnapshot.mousePosition,
        .delta = delta,
        .modifiers = pendingSnapshot.GetHeldModifiers() });
}

void WindowInputState::OnWindowFocusChanged(bool focused)
{
    pendingSnapshot.windowFocused = focused;

    if (!focused)
    {
        pendingSnapshot.mouseDelta = Vector2::Zero;
        pendingSnapshot.scrollDelta = Vector2::Zero;

        lastClickButton = MouseButton::None;
        consecutiveClickCount = 0;
    }

    pendingSnapshot.events.push_back(WindowFocusEvent{
        .focused = focused });
}

MouseButtonMask WindowInputState::BuildPressedMouseButtonMask() const
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
