#pragma once
#include <Core/Type/TypeAlias.h>

#include <Engine/UI/Widget/Element/TextBox.h>

#include <Core/Type/Ray/Ray.h>

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

protected:
    bool TryCommitText(const String& editingText, String& committedText) override;
    void OnTextChanged(const String& prevText, const String& currText) override;

private:
    bool TryParse(const String& text, int32& outValue) const;
    float GetModifiedDragSpeed(ModifierKey modifiers) const;

public:
    Ray<int32> onValueChanged;

private:
    Vector2 initPointerPosition = Vector2::Zero;
    double dragValue = 0.0;
    float dragSpeed = 1.0f;

    bool isPointerDown = false;
    bool isDragging = false;

    uint8 pendingClickCount = 0;
};

} // namespace URay
