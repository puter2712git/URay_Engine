#include "EditorRootWidget.h"

#include <Editor/Widget/Layer/DialogLayer.h>
#include <Editor/Widget/Layer/PopupLayer.h>
#include <Editor/Widget/Style/EditorStyles.h>
#include <Render/Rendering/Batch/UIDrawContext.h>

namespace URay
{

EditorRootWidget::EditorRootWidget(std::unique_ptr<Widget> mainContent)
    : UIRootWidget(std::move(mainContent))
{
    popupLayer = AddLayer<PopupLayer>();
    dialogLayer = AddLayer<DialogLayer>();
}

void EditorRootWidget::OnPaint(Render::UIDrawContext& context)
{
    context.AddFilledRect(rect, EditorStyle::RootBackground);
}

} // namespace URay
