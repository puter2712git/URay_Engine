#include "UIRootWidget.h"

namespace URay
{

UIRootWidget::UIRootWidget(std::unique_ptr<Widget> mainContent)
{
    AddChild(std::move(mainContent));
}

void UIRootWidget::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    for (const auto& child : GetChildren())
    {
        child->Arrange(rect);
    }
}

} // namespace URay
