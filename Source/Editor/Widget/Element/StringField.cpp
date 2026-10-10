#include "StringField.h"

#include <Core/Type/String.h>

namespace URay
{

void StringField::OnTextCommitted(
    const std::string& prevText,
    const std::string& committedText)
{
    valueChangedRay.Emit();
}

} // namespace URay
