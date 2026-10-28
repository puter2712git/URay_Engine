#include "PopupMenu.h"
#include <Core/Type/TypeAlias.h>

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

struct PopupMenu::MenuItem
{
    PopupMenu* owner = nullptr;
    Button* button = nullptr;
    int32 id = 0;

    void OnClicked() { owner->Emit("ItemSelected", id); }
};

PopupMenu::PopupMenu()
{
    URAY_DECLARE_RAY(ItemSelected, int32);
    surface = AddChild<Surface>();
    surface->SetBackgroundColor(EditorStyle::SurfaceHeader);
    surface->SetOutlineColor(EditorStyle::Border);
    surface->SetOutlineWidth(1.0f);
    surface->SetPadding(Padding(4.0f));

    layout = surface->AddChild<VerticalLayout>();
    layout->SetGap(2.0f);
    layout->SetCrossAxisAlignment(CrossAxisAlignment::Stretch);
}

PopupMenu::~PopupMenu()
{
    for (const auto& item : menuItems)
        item->button->UnregisterAll(item.get());
}

void PopupMenu::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);
    surface->Arrange(rect);
}

Button* PopupMenu::AddMenuItem(
    StringView text,
    int32 itemId)
{
    Button* button = CreateEditorButton(*layout, text);
    button->SetPreferredHeight(18.0f);

    Label* label = Cast<Label>(button->GetChildren()[0].get());
    label->SetTextStyle(EditorStyle::SmallText());

    auto item = std::make_unique<MenuItem>();
    item->owner = this;
    item->button = button;
    item->id = itemId;
    button->Register("Clicked", item.get(), &MenuItem::OnClicked);
    menuItems.push_back(std::move(item));

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
