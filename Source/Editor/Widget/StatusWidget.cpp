#include "StatusWidget.h"

#include "Engine/Engine.h"

#include "Core/Performance/PerformanceAnalytics.h"
#include "Core/Timer.h"

#include "Render/Rendering/Batch/UIDrawContext.h"

#include <imgui/imgui.h>

namespace URay
{

StatusWidget::StatusWidget()
{
    SetBackgroundColor(Color(0.08f, 0.12f, 0.20f, 1.0f));
    SetOutlineColor(Color(0.15f, 0.75f, 1.0f, 1.0f));
    SetOutlineWidth(3.0f);
    SetPadding(Padding(12.0f));
}

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

void StatusWidget::OnPaint(Render::UIDrawContext& context)
{
    Panel::OnPaint(context);

    const Rect& statusRect = GetRect();

    const Rect outerRect = {
        .position = statusRect.position + Vector2(20.0f, 20.0f),
        .size = Vector2(220.0f, 120.0f),
    };

    // 기준 배경: clip 없이 전체 표시
    context.AddFilledRect(
        outerRect,
        Color(0.15f, 0.15f, 0.18f, 1.0f));

    const Rect firstClip = {
        .position = outerRect.position + Vector2(30.0f, 20.0f),
        .size = Vector2(130.0f, 75.0f),
    };

    // 큰 초록 Rect를 그리지만 firstClip 안에서만 보여야 함
    context.PushClipRect(firstClip);
    context.AddFilledRect(
        outerRect,
        Color(0.0f, 0.9f, 0.25f, 1.0f));

    const Rect secondClip = {
        .position = firstClip.position + Vector2(25.0f, 15.0f),
        .size = Vector2(60.0f, 35.0f),
    };

    // 큰 빨강 Rect를 그리지만 secondClip 안에서만 보여야 함
    context.PushClipRect(secondClip);
    context.AddFilledRect(
        outerRect,
        Color(1.0f, 0.1f, 0.1f, 1.0f));
    context.PopClipRect();

    context.PopClipRect();
}

} // namespace URay
