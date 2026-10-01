#include "StatusPanel.h"

#include "Editor/Widget/Button.h"
#include "Editor/Widget/Label.h"

#include "Engine/Asset/EngineAsset.h"
#include "Engine/Engine.h"

#include "Core/Log/Log.h"
#include "Core/Performance/PerformanceAnalytics.h"
#include "Core/Timer.h"

#include "Render/Rendering/Batch/UIDrawContext.h"

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
                                     { Logger::Log("Button Clicked!"); });
    AddChild(std::move(newButton));
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
}

void StatusPanel::OnUpdate(float deltaTime)
{
    const Timer& timer = gEngine->GetTimer();
    fpsLabel->SetText(std::format("FPS: {} ({:.2f} ms)", timer.GetFPS(), timer.GetDeltaTime() * 1000.0));
}

} // namespace URay
