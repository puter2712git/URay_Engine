#include "InputSystem.h"

namespace URay
{

void InputSystem::BeginFrame()
{
    mainWindowState.BeginFrame();

    for (auto& [window, state] : windowStates)
    {
        state->BeginFrame();
    }
}

void InputSystem::PublishSnapshot(uint64 frameIndex)
{
    mainWindowState.PublishSnapshot(frameIndex);

    for (auto& [window, state] : windowStates)
    {
        state->PublishSnapshot(frameIndex);
    }
}

void InputSystem::OnKey(KeyCode key, KeyAction action, ModifierKey modifiers)
{
    mainWindowState.OnKey(key, action, modifiers);
}

void InputSystem::OnMouseButton(MouseButton button, KeyAction action, ModifierKey modifiers)
{
    mainWindowState.OnMouseButton(button, action, modifiers);
}

void InputSystem::OnCursorMoved(Vector2 position)
{
    mainWindowState.OnCursorMoved(position);
}

void InputSystem::OnTextInput(char32_t codepoint)
{
    mainWindowState.OnTextInput(codepoint);
}

void InputSystem::OnScroll(Vector2 delta)
{
    mainWindowState.OnScroll(delta);
}

void InputSystem::OnWindowFocusChanged(bool focused)
{
    mainWindowState.OnWindowFocusChanged(focused);
}

WindowInputState* InputSystem::RegisterWindow(Window& window)
{
    if (windowStates.contains(&window))
        return nullptr;

    auto state = std::make_unique<WindowInputState>();
    WindowInputState* statePtr = state.get();

    windowStates.emplace(&window, std::move(state));
    return statePtr;
}

bool InputSystem::UnregisterWindow(Window& window)
{
    return windowStates.erase(&window) != 0;
}

WindowInputState* InputSystem::FindWindowState(Window& window) const
{
    const auto it = windowStates.find(&window);
    if (it == windowStates.end())
        return nullptr;

    return it->second.get();
}

} // namespace URay
