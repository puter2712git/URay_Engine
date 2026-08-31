#pragma once

#include "Render/ImGui/ImGuiDrawable.h"

#include <vector>

namespace URay
{

class Widget;

class WidgetDrawer final : public Render::ImGuiDrawable
{
public:
    WidgetDrawer(const std::vector<Widget*>& widgets);
    ~WidgetDrawer() override;

public:
    void DrawImGui() override;

private:
    std::vector<Widget*> widgets;
};

} // namespace URay
