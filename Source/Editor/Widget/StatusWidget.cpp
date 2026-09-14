#include "StatusWidget.h"

#include "Engine/Engine.h"

#include "Core/Performance/PerformanceAnalytics.h"
#include "Core/Timer.h"

#include <imgui/imgui.h>

namespace URay
{

StatusWidget::StatusWidget() = default;

StatusWidget::~StatusWidget() = default;

void StatusWidget::OnDraw()
{
    ApplyRect();

    ImGui::Begin("Status", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    Timer& timer = gEngine->GetTimer();

    ImGui::Text("FPS: %d", timer.GetFPS());
    ImGui::Text("%.4f ms", timer.GetDeltaTime() * 1000);

    const PerformanceAnalytics& analytics = gEngine->GetPerformanceAnalytics();
    const auto& samples = analytics.GetCompletedSamples();

    if (!samples.empty())
    {
        ImGui::Separator();

        if (ImGui::BeginTable("CpuScopes", 2,
                              ImGuiTableFlags_SizingStretchProp))
        {
            for (const ScopeSample& sample : samples)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(sample.name.data());

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%.2f ms", sample.durationMs);
            }

            ImGui::EndTable();
        }
    }

    ImGui::End();
}

} // namespace URay
