#include "TextBox.h"

#include <Core/Type/String.h>
#include <Engine/Engine.h>
#include <Render/RenderSystem.h>
#include <Render/Rendering/Font/FontSystem.h>
#include <Render/ResourceManager.h>

#include <algorithm>

namespace URay
{

namespace
{
bool IsWordCharacter(char32_t codepoint)
{
    if (codepoint >= U'a' && codepoint <= U'z')
        return true;

    if (codepoint >= U'A' && codepoint <= U'Z')
        return true;

    if (codepoint >= U'0' && codepoint <= U'9')
        return true;

    if (codepoint == U'_')
        return true;

    return codepoint > 0x7F;
}
} // namespace

TextBox::TextBox(const std::string& text) : text(text) {}

EventReply TextBox::OnPointerEnter()
{
    return EventReply{
        .cursor = isReadonly
                      ? CursorType::ARROW
                      : CursorType::TEXT
    };
}

EventReply TextBox::OnPointerLeave()
{
    return EventReply{
        .cursor = CursorType::ARROW
    };
}

EventReply TextBox::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left || isReadonly)
        return {};

    BeginEdit();

    if (event.clickCount == 3)
    {
        SelectAll();
    }
    else
    {
        const Rect content = GetContentRect();
        const float contentX = event.position.x - content.position.x;
        const size_t clickedCaretIndex = GetCaretIndexAtX(contentX);

        if (event.clickCount == 1)
        {
            caretIndex = clickedCaretIndex;
            ClearSelection();
        }
        else if (event.clickCount == 2)
        {
            SelectWordAt(clickedCaretIndex);
        }
    }

    return EventReply{
        .handled = true,
        .requestFocus = true
    };
}

EventReply TextBox::OnTextInput(const TextInputEvent& event)
{
    if (!isEditing)
        return {};

    if (HasSelection())
        DeleteSelection();

    editingText.insert(caretIndex, 1, event.codepoint);
    ++caretIndex;
    ClearSelection();

    RefreshText();

    return EventReply{
        .handled = true
    };
}

EventReply TextBox::OnKeyDown(const KeyEvent& event)
{
    if (!isEditing)
        return {};

    const bool extendSelection =
        (event.modifiers & ModifierKey::Shift) != ModifierKey::None;

    if (event.key == KeyCode::Backspace)
    {
        if (HasSelection())
        {
            DeleteSelection();
            RefreshText();
        }
        else if (caretIndex > 0)
        {
            editingText.erase(caretIndex - 1, 1);
            --caretIndex;
            ClearSelection();
            RefreshText();
        }

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

    if (event.key == KeyCode::Left)
    {
        if (extendSelection)
        {
            if (caretIndex > 0)
                --caretIndex;
        }
        else if (HasSelection())
        {
            caretIndex = GetSelectionStart();
            ClearSelection();
        }
        else if (caretIndex > 0)
        {
            --caretIndex;
            ClearSelection();
        }

        return EventReply{
            .handled = true
        };
    }
    if (event.key == KeyCode::Right)
    {
        if (extendSelection)
        {
            if (caretIndex < editingText.size())
                ++caretIndex;
        }
        else if (HasSelection())
        {
            caretIndex = GetSelectionEnd();
            ClearSelection();
        }
        else if (caretIndex < editingText.size())
        {
            ++caretIndex;
            ClearSelection();
        }

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

    if (IsEditing() && IsFocused() && HasSelection())
    {
        Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
        Render::FontSystem& fontSystem =
            renderSystem.GetResourceManager().GetFontSystem();

        const std::string prefixBeforeSelection =
            UTF8::Encode(editingText.substr(0, GetSelectionStart()));

        const std::string prefixThroughSelection =
            UTF8::Encode(editingText.substr(0, GetSelectionEnd()));

        const Vector2 selectionStartSize = fontSystem.MeasureText(
            prefixBeforeSelection,
            textStyle.fontHandle,
            textStyle.pixelHeight);

        const Vector2 selectionEndSize = fontSystem.MeasureText(
            prefixThroughSelection,
            textStyle.fontHandle,
            textStyle.pixelHeight);

        Rect selectionRect;
        selectionRect.position = Vector2(
            content.position.x + selectionStartSize.x,
            content.position.y);
        selectionRect.size = Vector2(
            selectionEndSize.x - selectionStartSize.x,
            static_cast<float>(textStyle.pixelHeight));

        context.AddFilledRect(selectionRect, EditorStyle::TextSelection);
    }

    context.AddText(content.position, text, textStyle);
}

void TextBox::OnPaintForeground(Render::UIDrawContext& context)
{
    if (!IsEditing() || !IsFocused() || IsReadonly())
        return;

    const Rect content = GetContentRect();

    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Render::FontSystem& fontSystem = renderSystem.GetResourceManager().GetFontSystem();

    const std::string prefix = UTF8::Encode(editingText.substr(0, caretIndex));
    const Vector2 prefixSize = fontSystem.MeasureText(prefix, textStyle.fontHandle, textStyle.pixelHeight);

    Rect caretRect;
    caretRect.position = Vector2(content.position.x + prefixSize.x, content.position.y);
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

    caretIndex = editingText.size();
    selectionAnchor = caretIndex;
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

    const std::string prevText = GetText();

    if (prevText == originalText)
        return;

    SetText(originalText);
    OnTextChanged(prevText, originalText);
}

void TextBox::RefreshText()
{
    const std::string prevText = GetText();
    const std::string currText = UTF8::Encode(editingText);

    if (prevText == currText)
        return;

    SetText(currText);
    OnTextChanged(prevText, currText);
}

void TextBox::SelectAll()
{
    BeginEdit();

    selectionAnchor = 0;
    caretIndex = editingText.size();
}

size_t TextBox::GetSelectionStart() const
{
    return std::min(caretIndex, selectionAnchor);
}

size_t TextBox::GetSelectionEnd() const
{
    return std::max(caretIndex, selectionAnchor);
}

bool TextBox::HasSelection() const
{
    return caretIndex != selectionAnchor;
}

void TextBox::ClearSelection()
{
    selectionAnchor = caretIndex;
}

void TextBox::DeleteSelection()
{
    if (!HasSelection())
        return;

    const size_t start = GetSelectionStart();
    const size_t count = GetSelectionEnd() - start;

    editingText.erase(start, count);
    caretIndex = start;
    selectionAnchor = start;
}

float TextBox::GetPrefixWidth(size_t characterCount) const
{
    const size_t clampedCount = std::min(characterCount, editingText.size());

    const std::string prefix = UTF8::Encode(editingText.substr(0, clampedCount));

    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Render::FontSystem& fontSystem = renderSystem.GetResourceManager().GetFontSystem();

    Vector2 prefixSize = fontSystem.MeasureText(
        prefix,
        textStyle.fontHandle,
        textStyle.pixelHeight);

    return prefixSize.x;
}

size_t TextBox::GetCaretIndexAtX(float contentX) const
{
    if (contentX <= 0.0f)
        return 0;

    float previousBoundaryX = 0.0f;

    for (size_t index = 1; index <= editingText.size(); ++index)
    {
        const float currentBoundaryX = GetPrefixWidth(index);
        const float midpointX = (previousBoundaryX + currentBoundaryX) * 0.5f;

        if (contentX < midpointX)
            return index - 1;

        previousBoundaryX = currentBoundaryX;
    }

    return editingText.size();
}

void TextBox::SelectWordAt(size_t caretPosition)
{
    if (editingText.empty())
    {
        caretIndex = 0;
        selectionAnchor = 0;
        return;
    }

    size_t characterIndex = std::min(caretPosition, editingText.size() - 1);

    if (!IsWordCharacter(editingText[characterIndex]))
    {
        if (characterIndex > 0 && IsWordCharacter(editingText[characterIndex - 1]))
        {
            --characterIndex;
        }
        else
        {
            caretIndex = caretPosition;
            ClearSelection();
            return;
        }
    }

    size_t selectionStart = characterIndex;
    while (selectionStart > 0 && IsWordCharacter(editingText[selectionStart - 1]))
    {
        --selectionStart;
    }

    size_t selectionEnd = characterIndex + 1;
    while (selectionEnd < editingText.size() && IsWordCharacter(editingText[selectionEnd]))
    {
        ++selectionEnd;
    }

    selectionAnchor = selectionStart;
    caretIndex = selectionEnd;
}

} // namespace URay
