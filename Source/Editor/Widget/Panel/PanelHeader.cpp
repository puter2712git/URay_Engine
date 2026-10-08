#include "PanelHeader.h"

#include "Editor/Widget/Element/Label.h"

#include "Engine/Asset/EngineAsset.h"

namespace URay
{

PanelHeader::PanelHeader(const std::string& name)
{
    label = AddChild<Label>(name);
    label->SetTextStyle({
        .fontHandle = EngineAsset::EditorFont,
        .pixelHeight = 12,
        .color = Color::White,
    });
}

void PanelHeader::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    Rect labelRect = {};
    labelRect.position = rect.position;
    labelRect.size = rect.size;

    label->Arrange(labelRect);
}

void PanelHeader::OnPaint(Render::UIDrawContext& context)
{
    Widget::OnPaint(context);

    context.AddFilledRect(rect, Color(0.2f, 0.2f, 0.2f, 1.0f));
}

} // namespace URay
