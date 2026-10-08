#include "ViewportToolbar.h"

#include <Editor/Widget/Element/Button.h>
#include <Editor/Widget/Layout/HorizontalLayout.h>

#include <Engine/Asset/EngineAsset.h>

namespace URay
{

ViewportToolbar::ViewportToolbar()
{
    auto gizmoHorizontalLayout = std::make_unique<HorizontalLayout>();
    gizmoModeButtonLayout = gizmoHorizontalLayout.get();
    gizmoModeButtonLayout->SetPadding(Padding(5.0f, 5.0f));
    gizmoModeButtonLayout->SetGap(5.0f);

    auto translationButton = std::make_unique<Button>(EngineAsset::TranslationGizmoIconTexture);
    gizmoModeButtons[0] = translationButton.get();
    gizmoModeButtons[0]->SetDesiredSize(Vector2(30.0f, 30.0f));

    auto rotationButton = std::make_unique<Button>(EngineAsset::RotationGizmoIconTexture);
    gizmoModeButtons[1] = rotationButton.get();
    gizmoModeButtons[1]->SetDesiredSize(Vector2(30.0f, 30.0f));

    auto scaleButton = std::make_unique<Button>(EngineAsset::ScaleGizmoIconTexture);
    gizmoModeButtons[2] = scaleButton.get();
    gizmoModeButtons[2]->SetDesiredSize(Vector2(30.0f, 30.0f));

    auto spaceToggleButton = std::make_unique<Button>(EngineAsset::GizmoWorldSpaceIconTexture);
    gizmoSpaceToggleButton = spaceToggleButton.get();
    gizmoSpaceToggleButton->SetDesiredSize(Vector2(30.0f, 30.0f));

    auto viewToggleButton = std::make_unique<Button>(EngineAsset::ViewModeIconTexture);
    viewModeToggleButton = viewToggleButton.get();
    viewModeToggleButton->SetDesiredSize(Vector2(30.0f, 30.0f));

    AddChild(std::move(gizmoHorizontalLayout));

    gizmoModeButtonLayout->AddChild(std::move(translationButton));
    gizmoModeButtonLayout->AddChild(std::move(rotationButton));
    gizmoModeButtonLayout->AddChild(std::move(scaleButton));
    gizmoModeButtonLayout->AddChild(std::move(spaceToggleButton));
    gizmoModeButtonLayout->AddChild(std::move(viewToggleButton));
}

ViewportToolbar::~ViewportToolbar() = default;

void ViewportToolbar::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    gizmoModeButtonLayout->Arrange(rect);
}

} // namespace URay
