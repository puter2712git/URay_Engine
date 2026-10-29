#include "StatusPanel.h"

#include <Editor/Widget/Panel/PanelFactory.h>

#include <Editor/Widget/EditorWidgetFactory.h>

#include <Core/Performance/PerformanceViewModel.h>
#include <Core/TimeFlow/Timer.h>
#include <Editor/Editor.h>
#include <Editor/Widget/Element/StringField.h>
#include <Editor/Widget/Panel/Inspector/PropertyRow.h>
#include <Editor/Widget/Style/EditorStyles.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/EngineAsset.h>
#include <Engine/Asset/Texture/Texture.h>
#include <Engine/Engine.h>
#include <Engine/UI/Widget/Element/Checkbox.h>
#include <Engine/UI/Widget/Element/Label.h>
#include <Engine/UI/Widget/Layout/CrossAxisAlignment.h>
#include <Engine/UI/Widget/Layout/VerticalLayout.h>
#include <Render/RenderSystem.h>
#include <Render/Rendering/Batch/UIDrawContext.h>
#include <Render/Rendering/RenderPipeline.h>
#include <Render/ResourceManager.h>

namespace URay
{

URAY_REGISTER_PANEL(StatusPanel, "Status")

template <size_t Count>
void UpdateScopeLabels(
    const std::vector<ProfileScopeStatistics>& statistics,
    const std::array<Label*, Count>& labels)
{
    for (size_t index = 0; index < labels.size(); ++index)
    {
        Label* label = labels[index];

        if (index >= statistics.size())
        {
            label->SetText({});
            continue;
        }

        const ProfileScopeStatistics& scope = statistics[index];
        label->SetText(std::format(
            "{}  {:.2f} ms  avg {:.2f}  p95 {:.2f}",
            scope.name,
            scope.latestMs,
            scope.averageMs,
            scope.p95Ms));
    }
}

StatusPanel::StatusPanel()
{
    SetBackgroundColor(EditorStyle::Surface);
    SetPadding(Padding(2.0f, 2.0f));

    layout = AddChild<VerticalLayout>();
    layout->SetCrossAxisAlignment(CrossAxisAlignment::Stretch);
    layout->SetGap(2.0f);

    fpsLabel = CreateEditorLabel(*layout, "FPS:");
    fpsLabel->SetPreferredHeight(20.0f);
    fpsLabel->SetTextStyle(EditorStyle::SmallText());

    cullingStatsLabel = CreateEditorLabel(*layout, "Cull: collecting...");
    cullingStatsLabel->SetPreferredHeight(20.0f);
    cullingStatsLabel->SetTextStyle(EditorStyle::SmallText());

    cpuFrameLabel = CreateEditorLabel(*layout, "CPU: collecting...");
    cpuFrameLabel->SetPreferredHeight(20.0f);
    cpuFrameLabel->SetTextStyle(EditorStyle::SmallText());

    Label* cpuScopesHeader = CreateEditorLabel(*layout, "CPU scopes:");
    cpuScopesHeader->SetPreferredHeight(18.0f);
    cpuScopesHeader->SetTextStyle(EditorStyle::SmallText());

    for (Label*& scopeLabel : cpuScopeLabels)
    {
        scopeLabel = CreateEditorLabel(*layout);
        scopeLabel->SetPreferredHeight(18.0f);
        scopeLabel->SetTextStyle(EditorStyle::SmallText());
    }

    gpuFrameLabel = CreateEditorLabel(*layout, "GPU: collecting...");
    gpuFrameLabel->SetPreferredHeight(20.0f);
    gpuFrameLabel->SetTextStyle(EditorStyle::SmallText());

    Label* gpuPassesHeader = CreateEditorLabel(*layout, "GPU passes:");
    gpuPassesHeader->SetPreferredHeight(18.0f);
    gpuPassesHeader->SetTextStyle(EditorStyle::SmallText());

    for (Label*& scopeLabel : gpuScopeLabels)
    {
        scopeLabel = CreateEditorLabel(*layout);
        scopeLabel->SetPreferredHeight(18.0f);
        scopeLabel->SetTextStyle(EditorStyle::SmallText());
    }
}

void StatusPanel::Arrange(const Rect& rect)
{
    Panel::Arrange(rect);

    layout->Arrange(GetContentRect());
}

void StatusPanel::OnUpdate(float deltaTime)
{
    const Timer& timer = gEngine->GetTimer();
    fpsLabel->SetText(std::format("FPS: {} ({:.2f} ms)", timer.GetFPS(), timer.GetDeltaTime() * 1000.0));

    UpdateCullingStatsLabel();
    UpdateCpuFrameLabel();
    UpdateGpuFrameLabel();

    scopeRefreshElapsed += deltaTime;

    if (scopeRefreshElapsed < ScopeRefreshInterval)
        return;

    scopeRefreshElapsed = 0.0f;
    UpdateCpuScopeLabels();
    UpdateGpuScopeLabels();
}

void StatusPanel::UpdateCullingStatsLabel()
{
    const Render::RenderFrameStats& stats =
        gEngine->GetRenderSystem()
            .GetPipeline()
            .GetLatestStats();

    const auto& culling = stats.frustumCullingStats;

    const uint32 boundedCount =
        culling.visibleCount + culling.culledCount;

    const float culledPercent = boundedCount == 0
                                    ? 0.0f
                                    : static_cast<float>(culling.culledCount) * 100.0f /
                                          static_cast<float>(boundedCount);

    cullingStatsLabel->SetText(std::format(
        "Cull: {} visible / {} bounded | {} culled ({:.1f}%) | {} unbounded",
        culling.visibleCount,
        boundedCount,
        culling.culledCount,
        culledPercent,
        culling.unboundedCount));
}

void StatusPanel::UpdateCpuFrameLabel()
{
    const PerformanceViewModel& viewModel = gEditor->GetPerformanceViewModel();
    const ProfileFrame* frame = viewModel.GetLatestFrame(ProfileDomain::CPU);

    if (!frame)
    {
        cpuFrameLabel->SetText("CPU: collecting...");
    }
    else
    {
        cpuFrameLabel->SetText(std::format("CPU: {:.2f} ms", frame->capturedMs));
    }
}

void StatusPanel::UpdateGpuFrameLabel()
{
    const PerformanceViewModel& viewModel = gEditor->GetPerformanceViewModel();
    const ProfileFrame* frame = viewModel.GetLatestFrame(ProfileDomain::GPU);

    if (!frame)
    {
        gpuFrameLabel->SetText("GPU: collecting...");
        return;
    }

    gpuFrameLabel->SetText(std::format(
        "GPU: {:.2f} ms (submission {})",
        frame->capturedMs,
        frame->frameNumber));
}

void StatusPanel::UpdateCpuScopeLabels()
{
    const PerformanceViewModel& viewModel = gEditor->GetPerformanceViewModel();
    UpdateScopeLabels(viewModel.GetStatistics(ProfileDomain::CPU), cpuScopeLabels);
}

void StatusPanel::UpdateGpuScopeLabels()
{
    const PerformanceViewModel& viewModel = gEditor->GetPerformanceViewModel();
    UpdateScopeLabels(viewModel.GetStatistics(ProfileDomain::GPU), gpuScopeLabels);
}

} // namespace URay
