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
    void OnTextCommitted(const std::string& prevText, const std::string& committedText) override;

private:
    EventRay<> valueChangedRay;
};

} // namespace URay
