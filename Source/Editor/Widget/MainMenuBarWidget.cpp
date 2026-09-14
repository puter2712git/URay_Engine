#include "MainMenuBarWidget.h"

#include <imgui/imgui.h>

namespace URay
{

MainMenuBarWidget::MainMenuBarWidget() {}

MainMenuBarWidget::~MainMenuBarWidget() = default;

void MainMenuBarWidget::OnDraw()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

} // namespace URay
