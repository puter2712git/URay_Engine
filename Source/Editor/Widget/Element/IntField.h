#pragma once

#include <Engine/UI/Element/TextBox.h>

#include <Core/Ray/EventRay.h>
#include <Core/Type/Types.h>

namespace URay
{

class IntField final : public TextBox
{
public:
    IntField();

    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerMove(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

    void SetValue(int32 value);

    void SetDragSpeed(float speed) { dragSpeed = speed; }

    EventRay<int32>& GetValueChangedRay() { return onValueChanged; }

protected:
    bool TryCommitText(const std::string& editingText, std::string& committedText) override;
    void OnTextChanged(const std::string& prevText, const std::string& currText) override;

private:
    bool TryParse(const std::string& text, int32& outValue) const;
    float GetModifiedDragSpeed(ModifierKey modifiers) const;

private:
    EventRay<int32> onValueChanged;

    Vector2 initPointerPosition = Vector2::Zero;
    double dragValue = 0.0;
    float dragSpeed = 1.0f;

    bool isPointerDown = false;
    bool isDragging = false;

    uint8 pendingClickCount = 0;
};

} // namespace URay
