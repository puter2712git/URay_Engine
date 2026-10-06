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

StatusPanel::StatusPanel()
{
    SetBackgroundColor(Color(0.08f, 0.12f, 0.20f, 1.0f));
    SetOutlineColor(Color(0.15f, 0.75f, 1.0f, 1.0f));
    SetOutlineWidth(3.0f);
    SetPadding(Padding(12.0f));

    auto label = std::make_unique<Label>("Status");
    label->SetTextStyle({
        .fontHandle = EngineAsset::EditorFont,
        .pixelHeight = 18,
        .color = Color::White,
    });
    statusLabel = label.get();
    AddChild(std::move(label));

    label = std::make_unique<Label>("FPS:");
    label->SetTextStyle({
        .fontHandle = EngineAsset::EditorFont,
        .pixelHeight = 18,
        .color = Color::White,
    });
    fpsLabel = label.get();
    AddChild(std::move(label));

    auto newButton = std::make_unique<Button>("Test Button");
    button = newButton.get();
    button->GetOnClickRay().Register(this, [this]()
                                     { URAY_LOG("Button Clicked!"); });
    AddChild(std::move(newButton));

    auto newImage = std::make_unique<Image>(EngineAsset::TranslationGizmoIconTexture);
    testImage = newImage.get();
    AddChild(std::move(newImage));
}

StatusPanel::~StatusPanel()
{
    button->GetOnClickRay().UnregisterAll(this);
}

void StatusPanel::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    statusLabel->Arrange({ .position = rect.position, .size = Vector2(200.0f, 28.0f) });
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
