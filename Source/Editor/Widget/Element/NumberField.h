#pragma once

#include <Core/Type/Ray/RayEmitter.h>
#include <Core/Type/TypeAlias.h>
#include <Engine/UI/Widget/Element/TextBox.h>

#include <type_traits>

namespace URay
{

template <typename T>
class NumberField final : public TextBox, public RayEmitter
{
    static_assert(std::is_same_v<T, int32> || std::is_same_v<T, float>);
    using DragValue = std::conditional_t<std::is_same_v<T, int32>, double, float>;

public:
    NumberField();

    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerMove(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

    void SetValue(T value);
    void SetDragSpeed(float speed) { dragSpeed = speed; }

protected:
    bool TryCommitText(const String& editingText, String& committedText) override;
    void OnTextChanged(const String& prevText, const String& currText) override;

private:
    bool TryParse(const String& text, T& outValue) const;
    float GetModifiedDragSpeed(ModifierKey modifiers) const;

    Vector2 initPointerPosition = Vector2::Zero;
    DragValue dragValue = 0;
    float dragSpeed = std::is_same_v<T, int32> ? 1.0f : 0.1f;
    bool isPointerDown = false;
    bool isDragging = false;
    uint8 pendingClickCount = 0;
};

extern template class NumberField<int32>;
extern template class NumberField<float>;

} // namespace URay
