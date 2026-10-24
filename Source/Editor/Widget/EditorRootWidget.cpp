#include "EditorRootWidget.h"

#include <Editor/Widget/Layer/DialogLayer.h>
#include <Editor/Widget/Layer/PopupLayer.h>

namespace URay
{

EditorRootWidget::EditorRootWidget(std::unique_ptr<Widget> mainContent)
    : UIRootWidget(std::move(mainContent))
{
    popupLayer = AddLayer<PopupLayer>();
    dialogLayer = AddLayer<DialogLayer>();
}

} // namespace URay
