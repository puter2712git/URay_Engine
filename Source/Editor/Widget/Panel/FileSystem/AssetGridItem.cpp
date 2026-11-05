#include "AssetGridItem.h"

#include <Editor/Widget/EditorWidgetFactory.h>
#include <Editor/Widget/Element/AssetField.h>
#include <Editor/Widget/Style/EditorStyles.h>
#include <Engine/Asset/Texture/Texture.h>
#include <Engine/Engine.h>
#include <Engine/UI/UISystem.h>
#include <Engine/UI/Widget/Element/Image.h>
#include <Engine/UI/Widget/Element/Label.h>
#include <Engine/UI/Widget/Element/Surface.h>
#include <Engine/UI/Widget/Layout/VerticalLayout.h>

#include <utility>

namespace URay
{

AssetGridItem::AssetGridItem(AssetEntry entry) : entry(std::move(entry))
{
    URAY_DECLARE_RAY(Selected, AssetEntry);
    URAY_DECLARE_RAY(Clicked, AssetEntry);
    URAY_DECLARE_RAY(DoubleClicked, AssetEntry);

    surface = AddChild<Surface>();
    surface->SetOutlineWidth(1.0f);
    surface->SetOutlineColor(EditorStyle::Border);

    layout = surface->AddChild<VerticalLayout>();
    layout->SetPadding(Padding(8.0f, 0.0f, 0.0f, 0.0f));
    layout->SetGap(10.0f);
    layout->SetCrossAxisAlignment(CrossAxisAlignment::Center);

    thumbnailSurface = layout->AddChild<Surface>();
    thumbnailSurface->SetPreferredSize(Vector2(64.0f, 64.0f));

    const bool isTexture =
        this->entry.kind == AssetEntryKind::Asset &&
        this->entry.className == Texture::StaticType().name;

    thumbnailSurface->SetBackgroundColor(
        this->entry.kind == AssetEntryKind::Directory
            ? Color::FromRGB(0xC0, 0x90, 0x30)
            : EditorStyle::Separator);

    if (isTexture)
        thumbnailImage = thumbnailSurface->AddChild<Image>(this->entry.assetHandle);

    nameLabel = CreateEditorLabel(*layout, this->entry.displayName);
    nameLabel->SetTextStyle(EditorStyle::SmallText());
}

void AssetGridItem::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);
    surface->Arrange(rect);
}

void AssetGridItem::SetSelected(bool selected)
{
    this->selected = selected;
}

EventReply AssetGridItem::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left)
        return {};

    pendingClickCount = event.clickCount;
    pressPosition = event.position;
    isPointerDown = true;
    isDragging = false;

    return EventReply{
        .handled = true,
        .requestFocus = true,
        .capturePointer = true
    };
}

EventReply AssetGridItem::OnPointerMove(const PointerEvent& event)
{
    if (!isPointerDown)
        return {};

    // Only Asset can drag now.
    if (entry.kind != AssetEntryKind::Asset)
        return EventReply{ .handled = true };

    constexpr float dragThreshold = 4.0f;

    const float dx = event.position.x - pressPosition.x;
    const float dy = event.position.y - pressPosition.y;

    if (!isDragging && dx * dx + dy * dy >= dragThreshold * dragThreshold)
    {
        isDragging = true;
    }

    return EventReply{ .handled = true };
}

EventReply AssetGridItem::OnPointerUp(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left || !isPointerDown)
        return {};

    const bool doubleClicked = pendingClickCount == 2;
    const bool wasDragging = isDragging;

    pendingClickCount = 0;
    isPointerDown = false;
    isDragging = false;

    if (wasDragging)
    {
        UISystem& uiSystem = gEngine->GetUISystem();
        Window* window = uiSystem.FindWidgetWindow(*this);

        if (window)
        {
            Widget* hit = uiSystem.FindWidgetAt(*window, event.position);

            for (Widget* widget = hit; widget; widget = widget->GetParent())
            {
                if (AssetField* field = Cast<AssetField>(widget))
                {
                    field->AcceptAsset(entry.assetHandle);
                    break;
                }
            }
        }
    }

    if (!wasDragging && IsHovered())
    {
        Emit("Selected", entry);
        Emit("Clicked", entry);

        if (doubleClicked)
            Emit("DoubleClicked", entry);
    }

    return EventReply{
        .handled = true,
        .releasePointer = true
    };
}

void AssetGridItem::OnPointerCancel()
{
    pendingClickCount = 0;
    isPointerDown = false;
    isDragging = false;
}

Vector2 AssetGridItem::OnMeasure()
{
    return Vector2(96.0f, 112.0f);
}

void AssetGridItem::OnUpdate(float)
{
    const Color background = selected
                                 ? EditorStyle::ControlPressed
                             : IsHovered()
                                 ? EditorStyle::ControlHovered
                                 : EditorStyle::ControlNormal;

    surface->SetBackgroundColor(background);
}

} // namespace URay
