#include "StatusPanel.h"

#include "Editor/Widget/Element/Label.h"
#include "Editor/Widget/Layout/VerticalLayout.h"

#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/EngineAsset.h"
#include "Engine/Asset/Texture/Texture.h"
#include "Engine/Engine.h"

#include "Core/Log/LogSystem.h"
#include "Core/Performance/PerformanceAnalytics.h"
#include "Core/TimeFlow/Timer.h"

#include "Render/RenderSystem.h"
#include "Render/Rendering/Batch/UIDrawContext.h"
#include "Render/ResourceManager.h"

#include <imgui/imgui.h>

namespace URay
{

StatusPanel::StatusPanel() : Panel("Status")
{
    SetBackgroundColor(Color(0.1f, 0.1f, 0.1f));
    SetPadding(Padding(12.0f));

    layout = AddChild<VerticalLayout>();

    fpsLabel = layout->AddChild<Label>("FPS:");
    fpsLabel->SetDesiredSize(Vector2(200.0f, 100.0f));
    fpsLabel->SetTextStyle({
        .fontHandle = EngineAsset::EditorFont,
        .pixelHeight = 12,
        .color = Color::White,
    });
}

void StatusPanel::Arrange(const Rect& rect)
{
    Panel::Arrange(rect);

    Rect layoutRect;
    layoutRect.position = Vector2(rect.position.x, rect.position.y + 25.0f);
    layoutRect.size = Vector2(rect.size.x, rect.size.y - 25.0f);

    layout->Arrange(layoutRect);
}

void StatusPanel::OnUpdate(float deltaTime)
{
    const Timer& timer = gEngine->GetTimer();
    fpsLabel->SetText(std::format("FPS: {} ({:.2f} ms)", timer.GetFPS(), timer.GetDeltaTime() * 1000.0));
}

} // namespace URay
