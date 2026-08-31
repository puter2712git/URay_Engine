#include "WidgetDrawer.h"

#include "Editor/Widget/Widget.h"

namespace URay
{

WidgetDrawer::WidgetDrawer(const std::vector<Widget*>& widgets)
    : widgets(widgets)
{
}

WidgetDrawer::~WidgetDrawer()
{
}

void WidgetDrawer::DrawImGui()
{
    for (Widget* widget : widgets)
    {
        widget->Draw();
    }
}

} // namespace URay
