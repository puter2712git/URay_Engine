#include "BreadcrumbItem.h"
#include <Core/Type/TypeAlias.h>

#include <Editor/Widget/EditorWidgetFactory.h>

#include <Engine/UI/Widget/Element/Label.h>
#include <Engine/UI/Widget/Element/Surface.h>
#include <Editor/Widget/Style/EditorStyles.h>

namespace URay
{

BreadcrumbItem::BreadcrumbItem(VirtualPath path) : path(path)
{
    surface = AddChild<Surface>();
    surface->SetPadding(Padding(2.0f, 6.0f));

    const String text = this->path.GetFilename().empty()
                                 ? this->path.GetMountName() + "://"
                                 : this->path.GetFilename();

    label = CreateEditorLabel(*surface, text);
    label->SetTextStyle(EditorStyle::SmallText());
}

void BreadcrumbItem::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    surface->Arrange(rect);
}

EventReply BreadcrumbItem::OnPointerDown(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left)
        return {};

    return EventReply{
        .handled = true,
        .requestFocus = true,
        .capturePointer = true
    };
}

EventReply BreadcrumbItem::OnPointerUp(const PointerEvent& event)
{
    if (event.changedButton != MouseButton::Left)
        return {};

    if (IsHovered())
        onActivated.Emit(path);

    return EventReply{
        .handled = true,
        .releasePointer = true
    };
}

Vector2 BreadcrumbItem::OnMeasure()
{
    return surface->GetDesiredSize();
}

void BreadcrumbItem::OnUpdate(float deltaTime)
{
    surface->SetBackgroundColor(
        IsHovered()
            ? EditorStyle::ControlHovered
            : Color{});
}

} // namespace URay
