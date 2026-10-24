#include "StringField.h"

#include <Editor/Widget/EditorWidgetFactory.h>

#include <Core/Type/String.h>

namespace URay
{

StringField::StringField()
{
    ConfigureEditorTextBox(*this);
}

void StringField::OnTextChanged(
    const std::string& prevText,
    const std::string& currText)
{
    valueChangedRay.Emit();
}

void StringField::OnTextCommitted(
    const std::string& prevText,
    const std::string& committedText)
{
    valueCommittedRay.Emit();
}

} // namespace URay
