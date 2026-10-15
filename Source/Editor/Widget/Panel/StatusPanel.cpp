#include "StatusPanel.h"

#include <Core/Performance/PerformanceViewModel.h>
#include <Core/TimeFlow/Timer.h>
#include <Editor/Editor.h>
#include <Editor/Widget/Element/Checkbox.h>
#include <Editor/Widget/Element/Label.h>
#include <Editor/Widget/Element/StringField.h>
#include <Editor/Widget/Layout/CrossAxisAlignment.h>
#include <Editor/Widget/Layout/VerticalLayout.h>
#include <Editor/Widget/Panel/Inspector/PropertyRow.h>
#include <Editor/Widget/Style/EditorStyles.h>

#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/EngineAsset.h>
#include <Engine/Asset/Texture/Texture.h>
#include <Engine/Engine.h>

#include <Render/RenderSystem.h>
#include <Render/Rendering/Batch/UIDrawContext.h>
#include <Render/ResourceManager.h>

namespace URay
{

StatusPanel::StatusPanel() : Panel("Status")
{
    SetBackgroundColor(EditorStyle::Surface);
    SetOutlineWidth(2.0f);
    SetOutlineColor(EditorStyle::Border);
    SetPadding(Padding(2.0f, 2.0f));

    layout = AddChild<VerticalLayout>();
    layout->SetCrossAxisAlignment(CrossAxisAlignment::Stretch);
    layout->SetGap(2.0f);

    fpsLabel = layout->AddChild<Label>("FPS:");
    fpsLabel->SetPreferredHeight(20.0f);
    fpsLabel->SetTextStyle(EditorStyle::SmallText());

    cpuFrameLabel = layout->AddChild<Label>("CPU: collecting...");
    cpuFrameLabel->SetPreferredHeight(20.0f);
    cpuFrameLabel->SetTextStyle(EditorStyle::SmallText());

    for (Label*& scopeLabel : scopeLabels)
    {
        scopeLabel = layout->AddChild<Label>();
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

    UpdateCpuFrameLabel();

    scopeRefreshElapsed += deltaTime;

    if (scopeRefreshElapsed < ScopeRefreshInterval)
        return;

    scopeRefreshElapsed = 0.0f;
    UpdateScopeLabels();
}

void StatusPanel::UpdateCpuFrameLabel()
{
    const PerformanceViewModel& viewModel = gEditor->GetPerformanceViewModel();
    const ProfileFrame* frame = viewModel.GetLatestFrame();

    if (!frame)
    {
        cpuFrameLabel->SetText("CPU: collecting...");
    }
    else
    {
        cpuFrameLabel->SetText(std::format("CPU: {:.2f} ms", frame->capturedCpuMs));
    }
}

void StatusPanel::UpdateScopeLabels()
{
    const PerformanceViewModel& viewModel = gEditor->GetPerformanceViewModel();
    const std::vector<ProfileScopeStatistics>& statistics = viewModel.GetStatistics();

    for (size_t index = 0; index < scopeLabels.size(); ++index)
    {
        Label* label = scopeLabels[index];

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

} // namespace URay
