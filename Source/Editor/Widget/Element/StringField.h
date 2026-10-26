#pragma once
#include <Core/Type/TypeAlias.h>

#include <Engine/UI/Widget/Element/TextBox.h>

#include <Core/Type/Ray/Ray.h>

namespace URay
{

class StringField : public TextBox
{
public:
    StringField();

protected:
    void OnTextChanged(const String& prevText, const String& currText) override;
    void OnTextCommitted(const String& prevText, const String& committedText) override;

public:
    Ray<> onValueChanged;
    Ray<> onValueCommitted;

private:
};

} // namespace URay
