#include "UIInputRouter.h"

#include "Editor/Widget/Widget.h"

#include "Core/Input/InputManager.h"

namespace URay
{

void UIInputRouter::Process(Widget& root, const InputManager& input)
{
    const Vector2 position = Vector2(input.mouseX, input.mouseY);
    Widget* hitWidget = FindTopWidget(root, position);

    hoveredWidget = hitWidget;

    Widget* pointerTarget = pointerCapturedWidget ? pointerCapturedWidget : hitWidget;
    if (!pointerTarget)
        return;

    const bool hasPointerMoved = input.mouseDeltaX != 0.0 || input.mouseDeltaY != 0.0;

    if (hasPointerMoved)
    {
        const PointerEvent event = MakePointerEvent(input, MouseButton::None);
        pointerTarget->OnPointerMove(event);
    }

    if (input.GetMouseDown(GLFW_MOUSE_BUTTON_LEFT))
    {
        const PointerEvent event = MakePointerEvent(input, MouseButton::Left);
        const EventReply reply = pointerTarget->OnPointerDown(event);

        if (reply.capturePointer)
        {
            pointerCapturedWidget = pointerTarget;
            pointerTarget->pointerCaptured = true;
        }
    }

    if (input.GetMouseUp(GLFW_MOUSE_BUTTON_LEFT))
    {
        const PointerEvent event = MakePointerEvent(input, MouseButton::Left);
        const EventReply reply = pointerTarget->OnPointerUp(event);

        if (reply.releasePointer && pointerCapturedWidget)
        {
            pointerCapturedWidget->pointerCaptured = false;
            pointerCapturedWidget = nullptr;
        }
    }
}

Widget* UIInputRouter::FindTopWidget(Widget& root, const Vector2& position) const
{
    if (!root.HitTest(position))
        return nullptr;

    const auto& children = root.GetChildren();

    for (auto it = children.rbegin(); it != children.rend(); ++it)
    {
        Widget* child = it->get();

        if (Widget* hit = FindTopWidget(*child, position))
            return hit;
    }

    return &root;
}

PointerEvent UIInputRouter::MakePointerEvent(const InputManager& input, MouseButton mouseButton) const
{
    return PointerEvent{
        .pointerId = 0,
        .position = Vector2(input.mouseX, input.mouseY),
        .delta = Vector2(input.mouseDeltaX, input.mouseDeltaY),
        .changedButton = mouseButton,
        .pressedButtons = static_cast<uint8_t>(mouseButton),
        .modifiers = ModifierKey::None
    };
}

} // namespace URay
