#include "PopupMenu.h"
#include <Core/Type/Types.h>

#include <Engine/UI/Widget/Element/Button.h>
#include <Engine/UI/Widget/Element/Label.h>
#include <Editor/Widget/Element/Separator.h>
#include <Editor/Widget/EditorWidgetFactory.h>
#include <Engine/UI/Widget/Element/Surface.h>
#include <Engine/UI/Widget/Layout/VerticalLayout.h>
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
    StringView text,
    std::function<void()> onClick)
{
    Button* button = CreateEditorButton(*layout, text);
    button->SetPreferredHeight(18.0f);

    Label* label = Cast<Label>(button->GetChildren()[0].get());
    label->SetTextStyle(EditorStyle::SmallText());

    button->onClicked.Register(
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
