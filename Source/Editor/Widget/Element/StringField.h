#pragma once

#include <Core/Type/Ray/RayEmitter.h>
#include <Core/Type/TypeAlias.h>
#include <Engine/UI/Widget/Element/TextBox.h>

namespace URay
{

class StringField : public TextBox, public RayEmitter
{
public:
    StringField();

protected:
    void OnTextChanged(const String& prevText, const String& currText) override;
    void OnTextCommitted(const String& prevText, const String& committedText) override;

private:
};

} // namespace URay
