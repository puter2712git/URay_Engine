#include "RootWidget.h"

#include <Editor/Widget/Layer/DialogLayer.h>
#include <Editor/Widget/Layer/PopupLayer.h>

namespace URay
{

RootWidget::RootWidget(std::unique_ptr<Widget> mainContent)
{
    this->mainContent = mainContent.get();
    AddChild(std::move(mainContent));

    popupLayer = AddChild<PopupLayer>();
    dialogLayer = AddChild<DialogLayer>();
}

void RootWidget::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    mainContent->Arrange(rect);
    popupLayer->Arrange(rect);
    dialogLayer->Arrange(rect);
}

} // namespace URay
