#include "PopupMenu.h"

#include <Editor/Widget/Element/Button.h>
#include <Editor/Widget/Element/Label.h>
#include <Editor/Widget/Element/Separator.h>
#include <Editor/Widget/Element/Surface.h>
#include <Engine/UI/Layout/VerticalLayout.h>
#include <Editor/Widget/Style/EditorStyles.h>

#include <utility>

namespace URay
{

PopupMenu::PopupMenu()
{
    surface = AddChild<Surface>();
    surface->SetBackgroundColor(EditorStyle::SurfaceHeader);
    surface->SetOutlineColor(EditorStyle::Border);
    surface->SetOutlineWidth(1.0f);
    surface->SetPadding(Padding(4.0f));

    layout = surface->AddChild<VerticalLayout>();
    layout->SetGap(2.0f);
    layout->SetCrossAxisAlignment(CrossAxisAlignment::Stretch);
}

void PopupMenu::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);
    surface->Arrange(rect);
}

Button* PopupMenu::AddMenuItem(
    std::string_view text,
    std::function<void()> onClick)
{
    Button* button = layout->AddChild<Button>(text);
    button->SetPreferredHeight(18.0f);

    Label* label = Cast<Label>(button->GetChildren()[0].get());
    label->SetTextStyle(EditorStyle::SmallText());

    button->GetOnClickRay().Register(
        this,
        [onClick = std::move(onClick)]
        {
            onClick();
        });

    return button;
}

Separator* PopupMenu::AddMenuSeparator()
{
    Separator* separator = layout->AddChild<Separator>(SeparatorOrientation::Horizontal);
    return separator;
}

Vector2 PopupMenu::OnMeasure()
{
    return surface->GetDesiredSize();
}

} // namespace URay
