#include "StringField.h"

#include <Core/Type/String.h>
#include <Editor/Widget/EditorWidgetFactory.h>

namespace URay
{

StringField::StringField()
{
    URAY_DECLARE_RAY(ValueChanged);
    URAY_DECLARE_RAY(ValueCommitted);

    ConfigureEditorTextBox(*this);
}

void StringField::OnTextChanged(
    const String& prevText,
    const String& currText)
{
    onValueChanged.Emit();
    Emit("ValueChanged");
}

void StringField::OnTextCommitted(
    const String& prevText,
    const String& committedText)
{
    Emit("ValueCommitted");
}

} // namespace URay
