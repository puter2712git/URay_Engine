#include "StringField.h"

#include <Core/Type/String.h>

namespace URay
{

void StringField::OnTextChanged(
    const std::string& prevText,
    const std::string& currText)
{
    valueChangedRay.Emit();
}

} // namespace URay
