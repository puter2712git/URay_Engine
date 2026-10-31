#include "EditorRootWidget.h"

#include <Editor/Widget/Layer/DialogLayer.h>
#include <Editor/Widget/Layer/PopupLayer.h>
#include <Editor/Widget/Style/EditorStyles.h>
#include <Render/Rendering/Batch/UIDrawContext.h>

namespace URay
{

EditorRootWidget::EditorRootWidget(std::unique_ptr<Widget> mainContent)
{
    AddChild(std::move(mainContent));

    popupLayer = AddChild<PopupLayer>();
    dialogLayer = AddChild<DialogLayer>();
}

void EditorRootWidget::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    for (const auto& child : GetChildren())
    {
        child->Arrange(rect);
    }
}

void EditorRootWidget::OnPaint(Render::UIDrawContext& context)
{
    context.AddFilledRect(rect, EditorStyle::RootBackground);
}

} // namespace URay
