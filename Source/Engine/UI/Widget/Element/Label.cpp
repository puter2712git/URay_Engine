#include "Label.h"
#include <Core/Type/TypeAlias.h>

#include <Engine/Engine.h>

#include <Render/RenderSystem.h>
#include <Render/Rendering/Font/FontSystem.h>
#include <Render/ResourceManager.h>

namespace URay
{

Label::Label(StringView text) : text(text) {}

Label::~Label() = default;

Vector2 Label::OnMeasure()
{
    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Render::FontSystem& fontSystem = renderSystem.GetResourceManager().GetFontSystem();

    return fontSystem.MeasureText(text, style.fontHandle, style.pixelHeight);
}

void Label::OnPaint(Render::UIDrawContext& context)
{
    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Render::FontSystem& fontSystem = renderSystem.GetResourceManager().GetFontSystem();

    const Render::FontMetrics* metrics = fontSystem.GetMetrics(style.fontHandle, style.pixelHeight);
    if (!metrics)
        return;

    Vector2 textPosition = rect.position;
    const Vector2& textSize = GetDesiredSize();

    switch (horizontalAlignment)
    {
    case HorizontalTextAlignment::Left:
        break;

    case HorizontalTextAlignment::Center:
        textPosition.x += (rect.size.x - textSize.x) * 0.5f;
        break;

    case HorizontalTextAlignment::Right:
        textPosition.x += rect.size.x - textSize.x;
        break;
    }

    switch (verticalAlignment)
    {
    case VerticalTextAlignment::Top:
        break;

    case VerticalTextAlignment::Center:
        textPosition.y += (rect.size.y - textSize.y) * 0.5f;
        break;

    case VerticalTextAlignment::Bottom:
        textPosition.y += rect.size.y - textSize.y;
        break;
    }

    textPosition.y += metrics->ascender;

    context.AddText(textPosition, text, style);
}

} // namespace URay
