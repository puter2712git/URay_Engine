#include "ViewportToolbar.h"

#include <Engine/UI/Widget/Element/Button.h>
#include <Editor/Widget/EditorWidgetFactory.h>
#include <Engine/UI/Widget/Element/Image.h>
#include <Engine/UI/Widget/Layout/HorizontalLayout.h>

#include <Engine/Asset/EngineAsset.h>

namespace URay
{

ViewportToolbar::ViewportToolbar()
{
    gizmoModeButtonLayout = AddChild<HorizontalLayout>();
    gizmoModeButtonLayout->SetPadding(Padding(5.0f, 5.0f));
    gizmoModeButtonLayout->SetGap(5.0f);

    gizmoModeButtons[0] = CreateEditorIconButton(*gizmoModeButtonLayout, EngineAsset::TranslationGizmoIconTexture);
    gizmoModeButtons[0]->SetPreferredSize(Vector2(30.0f, 30.0f));

    gizmoModeButtons[1] = CreateEditorIconButton(*gizmoModeButtonLayout, EngineAsset::RotationGizmoIconTexture);
    gizmoModeButtons[1]->SetPreferredSize(Vector2(30.0f, 30.0f));

    gizmoModeButtons[2] = CreateEditorIconButton(*gizmoModeButtonLayout, EngineAsset::ScaleGizmoIconTexture);
    gizmoModeButtons[2]->SetPreferredSize(Vector2(30.0f, 30.0f));

    gizmoSpaceToggleButton = CreateEditorIconButton(*gizmoModeButtonLayout, EngineAsset::GizmoLocalSpaceIconTexture);
    gizmoSpaceToggleButton->SetPreferredSize(Vector2(30.0f, 30.0f));
    gizmoSpaceImage = static_cast<Image*>(gizmoSpaceToggleButton->GetChildren()[0].get());

    viewModeToggleButton = CreateEditorIconButton(*gizmoModeButtonLayout, EngineAsset::ViewModeIconTexture);
    viewModeToggleButton->SetPreferredSize(Vector2(30.0f, 30.0f));

    playControlLayout = AddChild<HorizontalLayout>();
    playControlLayout->SetPadding(Padding(5.0f, 5.0f));
    playControlLayout->SetGap(5.0f);

    playButton = CreateEditorButton(*playControlLayout, "Play");
    playButton->SetPreferredSize(Vector2(60.0f, 30.0f));

    stopButton = CreateEditorButton(*playControlLayout, "Stop");
    stopButton->SetPreferredSize(Vector2(60.0f, 30.0f));
}

ViewportToolbar::~ViewportToolbar() = default;

void ViewportToolbar::SetGizmoSpaceLocal(bool isLocal)
{
    gizmoSpaceImage->SetTexture(
        isLocal ? EngineAsset::GizmoLocalSpaceIconTexture : EngineAsset::GizmoWorldSpaceIconTexture);
}

void ViewportToolbar::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    gizmoModeButtonLayout->Arrange(rect);

    const Vector2 controlSize = playControlLayout->GetDesiredSize();

    Rect controlRect;
    controlRect.position = {
        rect.position.x + (rect.size.x - controlSize.x) * 0.5f,
        rect.position.y + (rect.size.y - controlSize.y) * 0.5f,
    };
    controlRect.size = controlSize;

    playControlLayout->Arrange(controlRect);
}

} // namespace URay
