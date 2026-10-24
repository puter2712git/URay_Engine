#pragma once

#include <Engine/UI/Element/TextBox.h>

#include <Core/Ray/EventRay.h>

namespace URay
{

class StringField : public TextBox
{
public:
    StringField();

    EventRay<>& GetValueChangedRay() { return onValueChanged; }
    EventRay<>& GetValueCommittedRay() { return onValueCommitted; }

protected:
    void OnTextChanged(const std::string& prevText, const std::string& currText) override;
    void OnTextCommitted(const std::string& prevText, const std::string& committedText) override;

private:
    EventRay<> onValueChanged;
    EventRay<> onValueCommitted;
};

} // namespace URay
