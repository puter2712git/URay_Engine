#include "Font.h"

namespace URay
{

URAY_REGISTER_CLASS(Font)

Font::Font(const std::vector<uint8>& data) : data(data) {}

Font::~Font() = default;

void Font::RegisterClass() {}

} // namespace URay
