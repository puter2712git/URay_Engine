#include "AssetGridItem.h"

#include <Editor/Widget/EditorWidgetFactory.h>
#include <Editor/Widget/Style/EditorStyles.h>
#include <Engine/Asset/Texture/Texture.h>
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
    URAY_DECLARE_RAY(Activated, AssetEntry);
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

    return EventReply{
        .handled = true,
        .requestFocus = true,
        .capturePointer = true
    };
}

EventReply AssetGridItem::OnPointerUp(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left)
        return {};

    if (IsHovered())
    {
        Emit("Selected", entry);
        Emit("Activated", entry);
    }

    return EventReply{
        .handled = true,
        .releasePointer = true
    };
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
