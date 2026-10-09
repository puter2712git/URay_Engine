#pragma once

#include <Editor/Widget/Element/TextBox.h>

#include <Core/Ray/EventRay.h>

namespace URay
{

class StringField : public TextBox
{
public:
    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnTextInput(const TextInputEvent& event) override;
    EventReply OnKeyDown(const KeyEvent& event) override;

    EventRay<>& GetValueChangedRay() { return valueChangedRay; }

private:
    void BeginEdit();
    void CommitEdit();
    void CancelEdit();

    void RefreshText();

private:
    std::u32string editingText;
    std::string originalText;

    bool isEditing = false;

    EventRay<> valueChangedRay;
};

} // namespace URay
