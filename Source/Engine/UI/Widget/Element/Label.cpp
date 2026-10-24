#include "Label.h"

#include <Engine/Engine.h>

#include <Render/RenderSystem.h>
#include <Render/Rendering/Font/FontSystem.h>
#include <Render/ResourceManager.h>

namespace URay
{

Label::Label(std::string_view text) : text(text) {}

Label::~Label() = default;

Vector2 Label::OnMeasure()
{
    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Render::FontSystem& fontSystem = renderSystem.GetResourceManager().GetFontSystem();

    return fontSystem.MeasureText(text, style.fontHandle, style.pixelHeight);
}

void Label::OnPaint(Render::UIDrawContext& context)
{
    context.AddText(rect.position, text, style);
}

} // namespace URay
