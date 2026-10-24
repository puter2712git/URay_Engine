#include "PanelHeader.h"

#include <Editor/Widget/EditorWidgetFactory.h>

#include <Engine/UI/Widget/Element/Label.h>
#include <Engine/UI/Widget/Layout/HorizontalLayout.h>
#include <Editor/Widget/Style/EditorStyles.h>

#include <Engine/Asset/EngineAsset.h>

namespace URay
{

PanelHeader::PanelHeader(const std::string& name)
{
    layout = AddChild<HorizontalLayout>();
    layout->SetCrossAxisAlignment(CrossAxisAlignment::Center);
    layout->SetPadding(Padding(5.0f, 0.0f));

    Label* label = CreateEditorLabel(*layout, name);
}

void PanelHeader::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    layout->Arrange(rect);
}

void PanelHeader::OnPaint(Render::UIDrawContext& context)
{
    Widget::OnPaint(context);

    context.AddFilledRect(rect, EditorStyle::SurfaceHeader);
}

} // namespace URay
