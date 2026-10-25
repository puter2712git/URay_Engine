#pragma once
#include <Core/Type/Types.h>

#include <Engine/UI/Widget/Element/TextBox.h>

#include <Core/Type/Ray.h>

namespace URay
{

class NumberField final : public TextBox
{
public:
    NumberField();

    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerMove(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

    void SetValue(float value);

    void SetDragSpeed(float speed) { dragSpeed = speed; }

protected:
    bool TryCommitText(const String& editingText, String& committedText) override;
    void OnTextChanged(const String& prevText, const String& currText) override;

private:
    bool TryParse(const String& text, float& outValue);
    String FormatValue(float value);

    float GetModifiedDragSpeed(ModifierKey modifiers) const;

public:
    Ray<float> onValueChanged;

private:
    Vector2 initPointerPosition = Vector2::Zero;
    float dragValue = 0.0f;
    float dragSpeed = 0.1f;

    bool isPointerDown = false;
    bool isDragging = false;

    uint8 pendingClickCount = 0;
};

} // namespace URay
