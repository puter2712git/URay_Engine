#include "FileGridItem.h"

#include <Editor/Widget/EditorWidgetFactory.h>

#include <Engine/UI/Widget/Element/Label.h>
#include <Engine/UI/Widget/Element/Surface.h>
#include <Engine/UI/Widget/Layout/VerticalLayout.h>
#include <Editor/Widget/Style/EditorStyles.h>

#include <utility>

namespace URay
{

FileGridItem::FileGridItem(VirtualFileEntry entry) : entry(std::move(entry))
{
    surface = AddChild<Surface>();
    surface->SetOutlineWidth(1.0f);
    surface->SetOutlineColor(EditorStyle::Border);

    layout = surface->AddChild<VerticalLayout>();
    layout->SetPadding(Padding(8.0f, 0.0f, 0.0f, 0.0f));
    layout->SetGap(10.0f);
    layout->SetCrossAxisAlignment(CrossAxisAlignment::Center);

    thumbnailSurface = layout->AddChild<Surface>();
    thumbnailSurface->SetPreferredSize(Vector2(64.0f, 64.0f));
    thumbnailSurface->SetBackgroundColor(
        this->entry.isDirectory
            ? Color::FromRGB(0xC0, 0x90, 0x30)
            : EditorStyle::Separator);

    nameLabel = CreateEditorLabel(*layout, this->entry.path.GetFilename());
    nameLabel->SetTextStyle(EditorStyle::SmallText());
}

void FileGridItem::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);
    surface->Arrange(rect);
}

EventReply FileGridItem::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left)
        return {};

    return EventReply{
        .handled = true,
        .requestFocus = true,
        .capturePointer = true
    };
}

EventReply FileGridItem::OnPointerUp(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left)
        return {};

    if (IsHovered())
        onActivated.Emit(entry);

    return EventReply{
        .handled = true,
        .releasePointer = true
    };
}

Vector2 FileGridItem::OnMeasure()
{
    return Vector2(96.0f, 112.0f);
}

void FileGridItem::OnUpdate(float)
{
    const Color background = IsHovered()
                                 ? EditorStyle::ControlHovered
                                 : EditorStyle::ControlNormal;

    surface->SetBackgroundColor(background);
}

} // namespace URay
