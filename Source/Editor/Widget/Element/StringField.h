#pragma once

#include <Engine/UI/Element/TextBox.h>

#include <Core/Ray/EventRay.h>

namespace URay
{

class StringField : public TextBox
{
public:
    StringField();

    EventRay<>& GetValueChangedRay() { return valueChangedRay; }
    EventRay<>& GetValueCommittedRay() { return valueCommittedRay; }

protected:
    void OnTextChanged(const std::string& prevText, const std::string& currText) override;
    void OnTextCommitted(const std::string& prevText, const std::string& committedText) override;

private:
    EventRay<> valueChangedRay;
    EventRay<> valueCommittedRay;
};

} // namespace URay
