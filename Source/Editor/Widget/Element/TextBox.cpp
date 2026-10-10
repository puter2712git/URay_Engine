#include "TextBox.h"

#include <Engine/Engine.h>

#include <Render/RenderSystem.h>
#include <Render/Rendering/Font/FontSystem.h>
#include <Render/ResourceManager.h>

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

Vector2 TextBox::OnMeasure()
{
    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Render::FontSystem& fontSystem = renderSystem.GetResourceManager().GetFontSystem();

    Vector2 textSize = fontSystem.MeasureText(text, textStyle.fontHandle, textStyle.pixelHeight);
    textSize.x += padding.left + padding.right + outlineWidth * 2.0f;
    textSize.y += padding.top + padding.bottom + outlineWidth * 2.0f;

    return textSize;
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
