#include "FileGridItem.h"

#include <Core/Math/Color.h>
#include <Editor/Widget/Style/EditorStyles.h>

namespace URay
{

FileGridItem::FileGridItem(VirtualFileEntry entry) : entry(std::move(entry)) {}

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
        activatedRay.Emit(entry);

    return EventReply{
        .handled = true,
        .releasePointer = true
    };
}

Vector2 FileGridItem::OnMeasure()
{
    return Vector2(96.0f, 112.0f);
}

void FileGridItem::OnPaint(Render::UIDrawContext& context)
{
    const Color background = IsHovered()
                                 ? EditorStyle::ControlHovered
                                 : EditorStyle::ControlNormal;

    context.AddFilledRect(rect, background);
    context.AddOutlineRect(rect, 1.0f, EditorStyle::Border);

    Rect thumbnailRect;
    thumbnailRect.position = rect.position + Vector2(16.0f, 8.0f);
    thumbnailRect.size = Vector2(64.0f, 64.0f);

    const Color thumbnailColor = entry.isDirectory
                                     ? Color::FromRGB(0xC0, 0x90, 0x30)
                                     : EditorStyle::Separator;

    context.AddFilledRect(thumbnailRect, thumbnailColor);

    context.AddText(
        rect.position + Vector2(6.0f, 82.0f),
        entry.path.GetFilename(),
        EditorStyle::SmallText());
}

} // namespace URay
