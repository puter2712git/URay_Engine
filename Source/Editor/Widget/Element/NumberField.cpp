#include "NumberField.h"

#include <charconv>
#include <cmath>
#include <format>

namespace URay
{

void NumberField::SetValue(float value)
{
    TextBox::SetText(FormatValue(value));
}

bool NumberField::TryCommitText(
    const std::string& editingText,
    std::string& committedText)
{
    float value = 0.0f;
    if (!TryParse(editingText, value))
        return false;

    committedText = FormatValue(value);
    return true;
}

void NumberField::OnTextChanged(
    const std::string& prevText,
    const std::string& currText)
{
    float value = 0.0f;
    if (!TryParse(currText, value))
        return;

    valueChangedRay.Emit(value);
}

bool NumberField::TryParse(const std::string& text, float& outValue)
{
    if (text.empty())
        return false;

    const char* begin = text.data();
    const char* end = begin + text.size();

    const auto [endPtr, error] = std::from_chars(
        begin,
        end,
        outValue,
        std::chars_format::general);

    return error == std::errc{} &&
           endPtr == end &&
           std::isfinite(outValue);
}

std::string NumberField::FormatValue(float value)
{
    return std::format("{}", value);
}

} // namespace URay
