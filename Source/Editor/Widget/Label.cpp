#include "Label.h"

namespace URay
{

Label::Label(std::string_view text) : text(text) {}

Label::~Label() = default;

void Label::OnPaint(Render::UIDrawContext& context)
{
    context.AddText(rect.position, text, style);
}

} // namespace URay
