#include "StatusPanel.h"

#include "Editor/Widget/Element/Button.h"
#include "Editor/Widget/Element/Image.h"
#include "Editor/Widget/Element/Label.h"

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

    fpsLabel = AddChild<Label>("FPS:");
    fpsLabel->SetTextStyle({
        .fontHandle = EngineAsset::EditorFont,
        .pixelHeight = 18,
        .color = Color::White,
    });

    button = AddChild<Button>("Test Button");
    button->GetOnClickRay().Register(this, [this]()
                                     { URAY_LOG("Button Clicked!"); });

    testImage = AddChild<Image>(EngineAsset::TranslationGizmoIconTexture);
}

StatusPanel::~StatusPanel()
{
    button->GetOnClickRay().UnregisterAll(this);
}

void StatusPanel::Arrange(const Rect& rect)
{
    Panel::Arrange(rect);

    fpsLabel->Arrange({ .position = rect.position + Vector2(0.0f, 32.0f), .size = Vector2(200.0f, 24.0f) });
    button->Arrange({ .position = rect.position + Vector2(0.0f, 50.0f), .size = Vector2(200.0f, 40.0f) });
    testImage->Arrange({ .position = rect.position + Vector2(0.0f, 80.0f), .size = Vector2(64, 64) });
}

void StatusPanel::OnUpdate(float deltaTime)
{
    const Timer& timer = gEngine->GetTimer();
    fpsLabel->SetText(std::format("FPS: {} ({:.2f} ms)", timer.GetFPS(), timer.GetDeltaTime() * 1000.0));
}

} // namespace URay
