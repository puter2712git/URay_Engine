#pragma once

#include <Editor/Widget/Element/TextBox.h>

#include <Core/Ray/EventRay.h>

namespace URay
{

class StringField : public TextBox
{
public:
    EventRay<>& GetValueChangedRay() { return valueChangedRay; }

protected:
    void OnTextChanged(const std::string& prevText, const std::string& currText) override;

private:
    EventRay<> valueChangedRay;
};

} // namespace URay
