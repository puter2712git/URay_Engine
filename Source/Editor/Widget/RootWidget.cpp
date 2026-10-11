#include "RootWidget.h"

#include <Editor/Widget/Layer/PopupLayer.h>

namespace URay
{

RootWidget::RootWidget(std::unique_ptr<Widget> mainContent)
{
    this->mainContent = mainContent.get();
    AddChild(std::move(mainContent));

    popupLayer = AddChild<PopupLayer>();
}

void RootWidget::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    mainContent->Arrange(rect);
    popupLayer->Arrange(rect);
}

} // namespace URay
