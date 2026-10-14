#pragma once

#include <Core/Input/InputEvent.h>
#include <Core/Input/InputSnapshot.h>

#include <chrono>
#include <vector>

namespace URay
{

class InputSystem
{
public:
    void BeginFrame();
    void PublishSnapshot(uint64 frameIndex);

    void OnKey(KeyCode key, KeyAction action, ModifierKey modifiers);
    void OnMouseButton(MouseButton button, KeyAction action, ModifierKey modifiers);
    void OnCursorMoved(Vector2 position);
    void OnTextInput(char32_t codepoint);
    void OnScroll(Vector2 delta);
    void OnWindowFocusChanged(bool focused);

    const InputSnapshot& GetSnapshot() const { return publishedSnapshot; }

private:
    MouseButtonMask BuildPressedMouseButtonMask() const;

private:
    InputSnapshot pendingSnapshot = {};
    InputSnapshot publishedSnapshot = {};

    std::chrono::steady_clock::time_point lastClickTime = {};
    Vector2 lastClickPosition = Vector2::Zero;
    MouseButton lastClickButton = MouseButton::None;
    uint8 consecutiveClickCount = 0;
};

} // namespace URay
