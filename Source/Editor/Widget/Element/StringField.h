#pragma once

#include <Engine/UI/Widget/Element/TextBox.h>

#include <Core/Type/Ray.h>

namespace URay
{

class StringField : public TextBox
{
public:
    StringField();

protected:
    void OnTextChanged(const std::string& prevText, const std::string& currText) override;
    void OnTextCommitted(const std::string& prevText, const std::string& committedText) override;

public:
    Ray<> onValueChanged;
    Ray<> onValueCommitted;

private:
};

} // namespace URay
