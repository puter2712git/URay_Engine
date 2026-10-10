#include "TextBox.h"

#include <Engine/Engine.h>

#include <Render/RenderSystem.h>
#include <Render/Rendering/Font/FontSystem.h>
#include <Render/ResourceManager.h>

#include <Core/Type/String.h>

namespace URay
{

TextBox::TextBox(const std::string& text) : text(text) {}

EventReply TextBox::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left || isReadonly)
        return {};

    BeginEdit();

    return EventReply{
        .handled = true,
        .requestFocus = true
    };
}

EventReply TextBox::OnTextInput(const TextInputEvent& event)
{
    if (!isEditing)
        return {};

    editingText.push_back(event.codepoint);
    RefreshText();

    return EventReply{
        .handled = true
    };
}

EventReply TextBox::OnKeyDown(const KeyEvent& event)
{
    if (!isEditing)
        return {};

    if (event.key == KeyCode::Backspace && !editingText.empty())
    {
        editingText.pop_back();
        RefreshText();

        return EventReply{
            .handled = true
        };
    }

    if (event.key == KeyCode::Enter)
    {
        CommitEdit();
        return EventReply{
            .handled = true
        };
    }

    if (event.key == KeyCode::Escape)
    {
        CancelEdit();
        return EventReply{
            .handled = true
        };
    }

    return {};
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

void TextBox::OnPaintForeground(Render::UIDrawContext& context)
{
    if (!IsEditing() || !IsFocused() || IsReadonly())
        return;

    const Rect content = GetContentRect();

    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Render::FontSystem& fontSystem = renderSystem.GetResourceManager().GetFontSystem();

    const Vector2 textSize = fontSystem.MeasureText(text, textStyle.fontHandle, textStyle.pixelHeight);

    Rect caretRect;
    caretRect.position = Vector2(content.position.x + textSize.x, content.position.y);
    caretRect.size = Vector2(1.0f, static_cast<float>(textStyle.pixelHeight));

    context.AddFilledRect(caretRect, EditorStyle::TextPrimary);
}

void TextBox::BeginEdit()
{
    if (isEditing)
        return;

    isEditing = true;
    originalText = GetText();
    editingText = UTF8::Decode(originalText);
}

void TextBox::CommitEdit()
{
    if (!isEditing)
        return;

    const std::string editingUTF8 = UTF8::Encode(editingText);
    std::string committedText;

    if (!TryCommitText(editingUTF8, committedText))
    {
        CancelEdit();
        return;
    }

    isEditing = false;

    if (committedText == originalText)
        return;

    SetText(committedText);
    OnTextCommitted(originalText, committedText);
}

void TextBox::CancelEdit()
{
    if (!isEditing)
        return;

    isEditing = false;
    SetText(originalText);
}

void TextBox::RefreshText()
{
    SetText(UTF8::Encode(editingText));
}

} // namespace URay
