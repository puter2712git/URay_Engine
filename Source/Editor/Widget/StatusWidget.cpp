#include "StatusWidget.h"

#include "Engine/Engine.h"

#include "Core/Timer.h"

#include <imgui/imgui.h>

namespace URay
{

StatusWidget::StatusWidget(Engine& engine)
    : engine(engine)
{
}

StatusWidget::~StatusWidget() = default;

void StatusWidget::OnDraw()
{
    ApplyRect();
    ImGui::Begin("Status", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImGui::Text("FPS: %d", engine.GetTimer()->GetFPS());
    ImGui::Text("%.4f ms", engine.GetTimer()->GetDeltaTime());

    ImGui::End();
}

} // namespace URay
