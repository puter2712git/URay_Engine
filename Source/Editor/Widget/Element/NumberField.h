#pragma once

#include <Editor/Widget/Element/TextBox.h>

#include <Core/Ray/EventRay.h>

namespace URay
{

class NumberField final : public TextBox
{
public:
    void SetValue(float value);

    EventRay<float>& GetValueChangedRay() { return valueChangedRay; }

protected:
    bool TryCommitText(const std::string& editingText, std::string& committedText) override;
    void OnTextChanged(const std::string& prevText, const std::string& currText) override;

private:
    bool TryParse(const std::string& text, float& outValue);
    std::string FormatValue(float value);

private:
    EventRay<float> valueChangedRay;
};

} // namespace URay
