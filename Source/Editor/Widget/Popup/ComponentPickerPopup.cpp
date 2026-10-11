#include "ComponentPickerPopup.h"

#include <Editor/Widget/Element/Button.h>
#include <Editor/Widget/Element/Surface.h>
#include <Editor/Widget/Layout/VerticalLayout.h>
#include <Engine/Component/ComponentFactory.h>

namespace URay
{

ComponentPickerPopup::ComponentPickerPopup(const Unit& unit)
{
    surface = AddChild<Surface>();
    surface->SetBackgroundColor(EditorStyle::SurfaceHeader);
    surface->SetOutlineColor(EditorStyle::Border);
    surface->SetOutlineWidth(1.0f);
    surface->SetPadding(Padding(4.0f));

    layout = surface->AddChild<VerticalLayout>();
    layout->SetGap(2.0f);
    layout->SetCrossAxisAlignment(CrossAxisAlignment::Stretch);

    std::vector<std::string> componentNames;

    for (const auto& [name, constructor] : ComponentFactory::GetRegisteredComponents())
    {
        componentNames.push_back(name);
    }

    std::sort(componentNames.begin(), componentNames.end());

    for (const std::string& typeName : componentNames)
    {
        Button* button = layout->AddChild<Button>(typeName);
        button->SetPreferredHeight(24.0f);

        button->GetOnClickRay().Register(
            this,
            [this, typeName]
            {
                componentSelectedRay.Emit(typeName);
            });
    }
}

void ComponentPickerPopup::Arrange(const Rect& rect)
{
    Widget::Arrange(rect);

    surface->Arrange(rect);
}

Vector2 ComponentPickerPopup::OnMeasure()
{
    return surface->GetDesiredSize();
}

} // namespace URay
