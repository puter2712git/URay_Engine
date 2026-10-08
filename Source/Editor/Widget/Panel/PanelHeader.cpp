#include "PanelHeader.h"

#include <Editor/Widget/Element/Label.h>
#include <Editor/Widget/Layout/HorizontalLayout.h>

#include <Engine/Asset/EngineAsset.h>

namespace URay
{

PanelHeader::PanelHeader(const std::string& name)
{
    layout = AddChild<HorizontalLayout>();
    layout->SetCrossAxisAlignment(CrossAxisAlignment::Center);
    layout->SetPadding(Padding(5.0f, 0.0f));

    Label* label = layout->AddChild<Label>(name);
    label->SetDesiredSize(Vector2(100.0f, 12.0f));
    label->SetTextStyle({
        .fontHandle = EngineAsset::EditorFont,
        .pixelHeight = 12,
        .color = Color::White,
    });
}

void PanelHeader::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    layout->Arrange(rect);
}

void PanelHeader::OnPaint(Render::UIDrawContext& context)
{
    Widget::OnPaint(context);

    context.AddFilledRect(rect, Color(0.05f, 0.05f, 0.05f, 1.0f));
}

} // namespace URay
