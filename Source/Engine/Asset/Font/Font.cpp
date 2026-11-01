#include "Font.h"
#include <Core/Type/TypeAlias.h>

namespace URay
{

URAY_REGISTER_CLASS(Font)

Font::Font(const Vector<uint8>& data) : data(data) {}

Font::~Font() = default;

void Font::RegisterClass() {}

} // namespace URay
