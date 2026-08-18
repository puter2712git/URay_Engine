#include "StatusWidget.h"

#include "Engine/Engine.h"

#include "Core/Timer.h"

namespace URay
{

StatusWidget::StatusWidget(Engine& engine)
    : engine(engine)
{
}

StatusWidget::~StatusWidget() = default;

void StatusWidget::OnDraw()
{
    ImGui::Begin("Status");

    ImGui::Text("FPS: %d", engine.GetTimer()->GetFPS());
    ImGui::Text("%.4f ms", engine.GetTimer()->GetDeltaTime());

    ImGui::End();
}

} // namespace URay
