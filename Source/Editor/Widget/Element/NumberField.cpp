#include "NumberField.h"
#include <Core/Type/TypeAlias.h>

#include <Editor/Widget/EditorWidgetFactory.h>

#include <Core/Math/Math.h>

#include <charconv>
#include <format>

namespace URay
{

NumberField::NumberField()
{
    URAY_DECLARE_RAY(ValueChanged, float);

    ConfigureEditorTextBox(*this);
}

EventReply NumberField::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left || IsReadonly())
        return {};

    pendingClickCount = event.clickCount;

    isPointerDown = true;
    isDragging = false;

    initPointerPosition = event.position;

    return EventReply{
        .handled = true,
        .requestFocus = true,
        .capturePointer = true
    };
}

EventReply NumberField::OnPointerMove(const PointerEvent& event)
{
    if (!isPointerDown)
        return {};

    if (!isDragging)
    {
        const float movedX = event.position.x - initPointerPosition.x;

        if (Math::Abs(movedX) < 4.0f)
        {
            return EventReply{
                .handled = true
            };
        }

        if (IsEditing())
        {
            CommitEdit();
        }

        if (!TryParse(GetText(), dragValue))
        {
            return EventReply{
                .handled = true
            };
        }

        isDragging = true;
    }

    dragValue += event.delta.x * GetModifiedDragSpeed(event.modifiers);

    SetValue(dragValue);
    onValueChanged.Emit(dragValue);
    Emit("ValueChanged", dragValue);

    return EventReply{
        .handled = true
    };
}

EventReply NumberField::OnPointerUp(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left || !isPointerDown)
        return {};

    const bool wasDragging = isDragging;

    isPointerDown = false;
    isDragging = false;

    if (!wasDragging && IsHovered())
    {
        ApplyTextClick(event.position, pendingClickCount);
    }

    pendingClickCount = 0;

    return EventReply{
        .handled = true,
        .releasePointer = true
    };
}

void NumberField::SetValue(float value)
{
    TextBox::SetText(FormatValue(value));
}

bool NumberField::TryCommitText(
    const String& editingText,
    String& committedText)
{
    float value = 0.0f;
    if (!TryParse(editingText, value))
        return false;

    committedText = FormatValue(value);
    return true;
}

void NumberField::OnTextChanged(
    const String& prevText,
    const String& currText)
{
    float value = 0.0f;
    if (!TryParse(currText, value))
        return;

    onValueChanged.Emit(value);
    Emit("ValueChanged", value);
}

bool NumberField::TryParse(const String& text, float& outValue)
{
    if (text.empty())
        return false;

    const char* begin = text.data();
    const char* end = begin + text.size();

    const auto [endPtr, error] = std::from_chars(
        begin,
        end,
        outValue,
        std::chars_format::general);

    return error == std::errc{} &&
           endPtr == end &&
           Math::IsFinite(outValue);
}

String NumberField::FormatValue(float value)
{
    return std::format("{}", value);
}

float NumberField::GetModifiedDragSpeed(ModifierKey modifiers) const
{
    if ((modifiers & ModifierKey::Shift) != ModifierKey::None)
    {
        return dragSpeed * 0.1f;
    }

    if ((modifiers & ModifierKey::Ctrl) != ModifierKey::None)
    {
        return dragSpeed * 10.0f;
    }

    return dragSpeed;
}

} // namespace URay
