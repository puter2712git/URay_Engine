#include "TextBox.h"

namespace URay
{

TextBox::TextBox(const std::string& text) : text(text) {}

EventReply TextBox::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left)
        return {};

    return EventReply{
        .handled = true,
        .requestFocus = true
    };
}

Rect TextBox::GetContentRect() const
{
    Rect contentRect;
    contentRect.position = Vector2(
        rect.position.x + padding.left + outlineWidth,
        rect.position.y + padding.top + outlineWidth);
    contentRect.size = Vector2(
        rect.size.x - padding.left - padding.right - outlineWidth * 2.0f,
        rect.size.y - padding.top - padding.bottom - outlineWidth * 2.0f);

    return contentRect;
}

void TextBox::OnPaint(Render::UIDrawContext& context)
{
    Color background = EditorStyle::ControlNormal;

    if (IsFocused())
    {
        background = EditorStyle::ControlPressed;
    }
    else if (IsHovered())
    {
        background = EditorStyle::ControlHovered;
    }

    context.AddFilledRect(rect, background);
    context.AddOutlineRect(rect, outlineWidth, EditorStyle::Border);

    const Rect content = GetContentRect();
    context.AddText(content.position, text, textStyle);
}

} // namespace URay
