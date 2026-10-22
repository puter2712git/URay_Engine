#include "ComponentSection.h"
#include <Core/Math/Math.h>

#include <Editor/Widget/Element/Label.h>
#include <Editor/Widget/Layout/VerticalLayout.h>

namespace URay
{

ComponentSection::ComponentSection(const std::string& title)
{
    titleLabel = AddChild<Label>(title);

    content = AddChild<VerticalLayout>();
    content->SetGap(2.0f);
    content->SetCrossAxisAlignment(CrossAxisAlignment::Stretch);
}

void ComponentSection::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    const float actualHeaderHeight = Math::Min(22.0f, rect.size.y);

    headerRect.position = rect.position;
    headerRect.size = Vector2(rect.size.x, actualHeaderHeight);

    const Vector2 titleSize = titleLabel->GetDesiredSize();

    Rect titleRect;
    titleRect.position = Vector2(
        headerRect.position.x + 6.0f,
        headerRect.position.y + (headerRect.size.y - titleSize.y) * 0.5f);
    titleRect.size = titleSize;
    titleLabel->Arrange(titleRect);

    Rect contentRect;
    contentRect.position = Vector2(
        rect.position.x,
        rect.position.y + actualHeaderHeight + 2.0f);
    contentRect.size = Vector2(
        rect.size.x,
        Math::Max(0.0f, rect.size.y - actualHeaderHeight - 2.0f * 2.0f));

    content->Arrange(contentRect);
}

Vector2 ComponentSection::OnMeasure()
{
    const Vector2 titleSize = titleLabel->GetDesiredSize();
    const Vector2 contentSize = content->GetDesiredSize();

    return Vector2(
        Math::Max(titleSize.x + 6.0f * 2.0f, contentSize.x),
        22.0f + 2.0f * 2.0f + contentSize.y);
}

void ComponentSection::OnPaint(Render::UIDrawContext& context)
{
    context.AddFilledRect(headerRect, EditorStyle::SurfaceHeader);

    Rect borderRect;
    borderRect.position = Vector2(
        headerRect.position.x,
        headerRect.position.y + headerRect.size.y - 1.0f);
    borderRect.size = Vector2(headerRect.size.x, 1.0f);

    context.AddFilledRect(borderRect, EditorStyle::Border);
}

} // namespace URay
