#include "DirectoryTreeItem.h"

#include <Editor/Widget/EditorWidgetFactory.h>

#include <Engine/UI/Element/Label.h>
#include <Engine/UI/Element/Surface.h>
#include <Engine/UI/Layout/HorizontalLayout.h>
#include <Editor/Widget/Style/EditorStyles.h>

#include <utility>

namespace URay
{

DirectoryTreeItem::DirectoryTreeItem(VirtualPath path, uint32 depth, bool expanded, bool selected)
    : path(std::move(path)), depth(depth), expanded(expanded), selected(selected)
{
    surface = AddChild<Surface>();

    layout = surface->AddChild<HorizontalLayout>();
    layout->SetPadding(Padding(0.0f, 0.0f, 0.0f, depth * 16.0f));
    layout->SetCrossAxisAlignment(CrossAxisAlignment::Center);

    disclosureLabel = CreateEditorLabel(*layout, expanded ? "v" : ">");
    disclosureLabel->SetPreferredWidth(16.0f);
    disclosureLabel->SetTextStyle(EditorStyle::SmallText());

    const std::string label = this->path.GetFilename().empty()
                                  ? this->path.GetMountName() + "://"
                                  : this->path.GetFilename();

    directoryLabel = CreateEditorLabel(*layout, label);
    directoryLabel->SetTextStyle(EditorStyle::SmallText());
}

void DirectoryTreeItem::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);
    surface->Arrange(rect);
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

    const Rect& disclosureRect = disclosureLabel->GetRect();

    if (event.position.x >= disclosureRect.position.x &&
        event.position.x < disclosureRect.position.x + disclosureRect.size.x)
    {
        onExpandRequested.Emit(path);
    }
    else
    {
        onActivated.Emit(path);
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

void DirectoryTreeItem::OnUpdate(float)
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

    surface->SetBackgroundColor(background);
}

} // namespace URay
