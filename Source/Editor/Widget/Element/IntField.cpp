#include "IntField.h"
#include <Core/Type/TypeAlias.h>

#include <Editor/Widget/EditorWidgetFactory.h>

#include <Core/Math/Math.h>

#include <algorithm>
#include <charconv>
#include <cmath>
#include <format>
#include <limits>

namespace URay
{

IntField::IntField()
{
    ConfigureEditorTextBox(*this);
}

EventReply IntField::OnPointerDown(const PointerEvent& event)
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
        .capturePointer = true,
    };
}

EventReply IntField::OnPointerMove(const PointerEvent& event)
{
    if (!isPointerDown)
        return {};

    if (!isDragging)
    {
        if (Math::Abs(event.position.x - initPointerPosition.x) < 4.0f)
            return EventReply{ .handled = true };

        if (IsEditing())
            CommitEdit();

        int32 currentValue = 0;
        if (!TryParse(GetText(), currentValue))
            return EventReply{ .handled = true };

        dragValue = static_cast<double>(currentValue);
        isDragging = true;
    }

    dragValue += event.delta.x * GetModifiedDragSpeed(event.modifiers);

    const double clampedValue = Math::Clamp(
        Math::Round(dragValue),
        static_cast<double>(std::numeric_limits<int32>::min()),
        static_cast<double>(std::numeric_limits<int32>::max()));
    const int32 newValue = static_cast<int32>(clampedValue);

    int32 previousValue = 0;
    if (TryParse(GetText(), previousValue) && previousValue == newValue)
        return EventReply{ .handled = true };

    SetValue(newValue);
    onValueChanged.Emit(newValue);

    return EventReply{ .handled = true };
}

EventReply IntField::OnPointerUp(const PointerEvent& event)
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
        .releasePointer = true,
    };
}

void IntField::SetValue(int32 value)
{
    TextBox::SetText(std::format("{}", value));
}

bool IntField::TryCommitText(
    const String& editingText,
    String& committedText)
{
    int32 value = 0;
    if (!TryParse(editingText, value))
        return false;

    committedText = std::format("{}", value);
    return true;
}

void IntField::OnTextChanged(
    const String&,
    const String& currText)
{
    int32 value = 0;
    if (TryParse(currText, value))
        onValueChanged.Emit(value);
}

bool IntField::TryParse(const String& text, int32& outValue) const
{
    if (text.empty())
        return false;

    const char* begin = text.data();
    const char* end = begin + text.size();

    const auto [endPtr, error] = std::from_chars(begin, end, outValue, 10);
    return error == std::errc{} && endPtr == end;
}

float IntField::GetModifiedDragSpeed(ModifierKey modifiers) const
{
    if ((modifiers & ModifierKey::Shift) != ModifierKey::None)
        return dragSpeed * 0.1f;

    if ((modifiers & ModifierKey::Ctrl) != ModifierKey::None)
        return dragSpeed * 10.0f;

    return dragSpeed;
}

} // namespace URay
