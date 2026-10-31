#include "WindowWidget.h"

namespace URay
{

WindowWidget::WindowWidget(Window& window) : window(window) {}

void WindowWidget::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    for (const auto& child : GetChildren())
    {
        child->Arrange(rect);
    }
}

} // namespace URay
