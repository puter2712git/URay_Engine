#include "StringField.h"
#include <Core/Type/TypeAlias.h>

#include <Editor/Widget/EditorWidgetFactory.h>

#include <Core/Type/String.h>

namespace URay
{

StringField::StringField()
{
    ConfigureEditorTextBox(*this);
}

void StringField::OnTextChanged(
    const String& prevText,
    const String& currText)
{
    onValueChanged.Emit();
}

void StringField::OnTextCommitted(
    const String& prevText,
    const String& committedText)
{
    onValueCommitted.Emit();
}

} // namespace URay
