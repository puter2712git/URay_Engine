#include "StatusPanel.h"

#include <Editor/Widget/Element/Label.h>
#include <Editor/Widget/Layout/VerticalLayout.h>
#include <Editor/Widget/Style/EditorStyles.h>

#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/EngineAsset.h>
#include <Engine/Asset/Texture/Texture.h>
#include <Engine/Engine.h>

#include <Core/Log/LogSystem.h>
#include <Core/Performance/PerformanceAnalytics.h>
#include <Core/TimeFlow/Timer.h>

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

    fpsLabel = layout->AddChild<Label>("FPS:");
    fpsLabel->SetDesiredSize(Vector2(200.0f, 100.0f));
    fpsLabel->SetTextStyle(EditorStyle::SmallText());
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
}

} // namespace URay
