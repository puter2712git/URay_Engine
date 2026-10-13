#pragma once

#include <Core/Input/InputEvent.h>
#include <Core/Input/InputSnapshot.h>

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
};

} // namespace URay
