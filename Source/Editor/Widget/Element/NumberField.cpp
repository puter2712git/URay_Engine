#include "NumberField.h"

#include <Core/Math/Math.h>
#include <Editor/Widget/EditorWidgetFactory.h>

#include <charconv>
#include <format>
#include <limits>

namespace URay
{

template <typename T>
NumberField<T>::NumberField()
{
    URAY_DECLARE_RAY(ValueChanged, T);
    ConfigureEditorTextBox(*this);
}

template <typename T>
EventReply NumberField<T>::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left || IsReadonly())
        return {};

    pendingClickCount = event.clickCount;
    isPointerDown = true;
    isDragging = false;
    initPointerPosition = event.position;
    return EventReply{ .handled = true, .requestFocus = true, .capturePointer = true };
}

template <typename T>
EventReply NumberField<T>::OnPointerMove(const PointerEvent& event)
{
    if (!isPointerDown)
        return {};

    if (!isDragging)
    {
        if (Math::Abs(event.position.x - initPointerPosition.x) < 4.0f)
            return EventReply{ .handled = true };

        if (IsEditing())
            CommitEdit();

        T currentValue = 0;
        if (!TryParse(GetText(), currentValue))
            return EventReply{ .handled = true };

        dragValue = static_cast<DragValue>(currentValue);
        isDragging = true;
    }

    dragValue += event.delta.x * GetModifiedDragSpeed(event.modifiers);

    if constexpr (std::is_same_v<T, int32>)
    {
        const double clampedValue = Math::Clamp(
            Math::Round(dragValue),
            static_cast<double>(std::numeric_limits<int32>::min()),
            static_cast<double>(std::numeric_limits<int32>::max()));
        const T newValue = static_cast<T>(clampedValue);
        T previousValue = 0;
        if (TryParse(GetText(), previousValue) && previousValue == newValue)
            return EventReply{ .handled = true };
        SetValue(newValue);
        Emit("ValueChanged", newValue);
    }
    else
    {
        SetValue(dragValue);
        Emit("ValueChanged", dragValue);
    }

    return EventReply{ .handled = true };
}

template <typename T>
EventReply NumberField<T>::OnPointerUp(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left || !isPointerDown)
        return {};

    const bool wasDragging = isDragging;
    isPointerDown = false;
    isDragging = false;
    if (!wasDragging && IsHovered())
        ApplyTextClick(event.position, pendingClickCount);
    pendingClickCount = 0;
    return EventReply{ .handled = true, .releasePointer = true };
}

template <typename T>
void NumberField<T>::OnPointerCancel()
{
    isPointerDown = false;
    isDragging = false;
    pendingClickCount = 0;
}

template <typename T>
void NumberField<T>::SetValue(T value)
{
    TextBox::SetText(std::format("{}", value));
}

template <typename T>
bool NumberField<T>::TryCommitText(const String& editingText, String& committedText)
{
    T value = 0;
    if (!TryParse(editingText, value))
        return false;
    committedText = std::format("{}", value);
    return true;
}

template <typename T>
void NumberField<T>::OnTextChanged(const String&, const String& currText)
{
    T value = 0;
    if (TryParse(currText, value))
        Emit("ValueChanged", value);
}

template <typename T>
bool NumberField<T>::TryParse(const String& text, T& outValue) const
{
    if (text.empty())
        return false;

    const char* begin = text.data();
    const char* end = begin + text.size();
    if constexpr (std::is_same_v<T, int32>)
    {
        const auto [endPtr, error] = std::from_chars(begin, end, outValue, 10);
        return error == std::errc{} && endPtr == end;
    }
    else
    {
        const auto [endPtr, error] = std::from_chars(begin, end, outValue, std::chars_format::general);
        return error == std::errc{} && endPtr == end && Math::IsFinite(outValue);
    }
}

template <typename T>
float NumberField<T>::GetModifiedDragSpeed(ModifierKey modifiers) const
{
    if ((modifiers & ModifierKey::Shift) != ModifierKey::None)
        return dragSpeed * 0.1f;
    if ((modifiers & ModifierKey::Ctrl) != ModifierKey::None)
        return dragSpeed * 10.0f;
    return dragSpeed;
}

template class NumberField<int32>;
template class NumberField<float>;

} // namespace URay
