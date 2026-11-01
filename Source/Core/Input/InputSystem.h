#pragma once

#include <Core/Input/WindowInputState.h>
#include <Core/Type/TypeAlias.h>

namespace URay
{

class Window;

class InputSystem
{
public:
    void PublishSnapshot(uint64 frameIndex);

    void OnKey(KeyCode key, KeyAction action, ModifierKey modifiers);
    void OnMouseButton(MouseButton button, KeyAction action, ModifierKey modifiers);
    void OnCursorMoved(Vector2 position);
    void OnTextInput(char32_t codepoint);
    void OnScroll(Vector2 delta);
    void OnWindowFocusChanged(bool focused);

    WindowInputState* RegisterWindow(Window& window);
    bool UnregisterWindow(Window& window);

    WindowInputState* FindWindowState(Window& window) const;

    const InputSnapshot& GetSnapshot() const { return mainWindowState.GetSnapshot(); }

private:
    WindowInputState mainWindowState;

    HashMap<Window*, UniquePtr<WindowInputState>> windowStates;
};

} // namespace URay
