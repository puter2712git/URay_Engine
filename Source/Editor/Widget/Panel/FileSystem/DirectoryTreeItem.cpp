#include "DirectoryTreeItem.h"

#include <Core/Math/Color.h>
#include <Editor/Widget/Style/EditorStyles.h>

namespace URay
{

DirectoryTreeItem::DirectoryTreeItem(VirtualPath path, uint32 depth, bool expanded, bool selected)
    : path(path), depth(depth), expanded(expanded), selected(selected) {}

void DirectoryTreeItem::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    disclosureRect.position = rect.position + Vector2(depth * 16.0f, 0.0f);
    disclosureRect.size = Vector2(16.0f, rect.size.y);
}

EventReply DirectoryTreeItem::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left)
        return {};

    return EventReply{
        .handled = true,
        .requestFocus = true,
        .capturePointer = true
    };
}

EventReply DirectoryTreeItem::OnPointerUp(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left)
        return {};

    if (!IsHovered())
    {
        return EventReply{
            .handled = true,
            .releasePointer = true
        };
    }

    if (event.position.x >= disclosureRect.position.x &&
        event.position.x < disclosureRect.position.x + disclosureRect.size.x)
    {
        expandRequestedRay.Emit(path);
    }
    else
    {
        activatedRay.Emit(path);
    }

    return EventReply{
        .handled = true,
        .releasePointer = true
    };
}

Vector2 DirectoryTreeItem::OnMeasure()
{
    return Vector2(0.0f, 20.0f);
}

void DirectoryTreeItem::OnPaint(Render::UIDrawContext& context)
{
    Color background = EditorStyle::Surface;

    if (selected)
    {
        background = EditorStyle::ControlPressed;
    }
    else if (IsHovered())
    {
        background = EditorStyle::ControlHovered;
    }

    context.AddFilledRect(rect, background);

    context.AddText(
        disclosureRect.position + Vector2(3.0f, 3.0f),
        expanded ? "v" : ">",
        EditorStyle::SmallText());

    const std::string label = path.GetFilename().empty()
                                  ? path.GetMountName() + "://"
                                  : path.GetFilename();

    context.AddText(
        rect.position + Vector2(depth * 16.0f + 18.0f, 3.0f),
        label,
        EditorStyle::SmallText());
}

} // namespace URay
