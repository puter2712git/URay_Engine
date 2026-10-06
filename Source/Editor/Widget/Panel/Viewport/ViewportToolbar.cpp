#include "ViewportToolbar.h"

#include "Editor/Widget/Element/Button.h"
#include "Editor/Widget/Layout/HorizontalLayout.h"

namespace URay
{

ViewportToolbar::ViewportToolbar()
{
    auto gizmoHorizontalLayout = std::make_unique<HorizontalLayout>();
    gizmoModeButtonLayout = gizmoHorizontalLayout.get();
    gizmoModeButtonLayout->SetPadding(Padding(5.0f, 5.0f));
    gizmoModeButtonLayout->SetGap(5.0f);

    auto translationButton = std::make_unique<Button>("Translation");
    gizmoModeButtons[0] = translationButton.get();
    gizmoModeButtons[0]->SetDesiredSize(Vector2(30.0f, 0.0f));

    auto rotationButton = std::make_unique<Button>("Rotation");
    gizmoModeButtons[1] = rotationButton.get();
    gizmoModeButtons[1]->SetDesiredSize(Vector2(30.0f, 0.0f));
    gizmoModeButtons[1]->SetDesiredSize(Vector2(30.0f, 0.0f));

    auto scaleButton = std::make_unique<Button>("Scale");
    gizmoModeButtons[2] = scaleButton.get();
    gizmoModeButtons[2]->SetDesiredSize(Vector2(30.0f, 0.0f));

    AddChild(std::move(gizmoHorizontalLayout));

    gizmoModeButtonLayout->AddChild(std::move(translationButton));
    gizmoModeButtonLayout->AddChild(std::move(rotationButton));
    gizmoModeButtonLayout->AddChild(std::move(scaleButton));
}

ViewportToolbar::~ViewportToolbar() = default;

void ViewportToolbar::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    gizmoModeButtonLayout->Arrange(rect);
}

} // namespace URay
