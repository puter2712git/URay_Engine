#include "ComponentPickerPopup.h"

#include <Editor/Widget/Element/Button.h>
#include <Editor/Widget/Element/Separator.h>
#include <Editor/Widget/Element/Surface.h>
#include <Editor/Widget/Layout/VerticalLayout.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/Script/ScriptAsset.h>
#include <Engine/Component/ComponentFactory.h>
#include <Engine/Component/ScriptComponent.h>
#include <Engine/Engine.h>

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
        if (name == ScriptComponent::StaticRuntimeType().name)
            continue;

        componentNames.push_back(name);
    }

    std::sort(componentNames.begin(), componentNames.end());

    for (const std::string& typeName : componentNames)
    {
        Button* button = layout->AddChild<Button>(typeName);
        button->SetPreferredHeight(24.0f);

        const ComponentPickerSelection selection = {
            .kind = ComponentPickerItemKind::NativeComponent,
            .componentTypeName = typeName
        };

        button->GetOnClickRay().Register(
            this,
            [this, selection]
            {
                componentSelectedRay.Emit(selection);
            });
    }

    AssetDatabase& database = gEngine->GetAssetSystem().GetDatabase();

    std::vector<ScriptAsset*> scripts = database.GetAssets<ScriptAsset>();

    std::sort(
        scripts.begin(),
        scripts.end(),
        [](const ScriptAsset* lhs, const ScriptAsset* rhs)
        {
            return lhs->GetName() < rhs->GetName();
        });

    if (!componentNames.empty() && !scripts.empty())
    {
        layout->AddChild<Separator>(SeparatorOrientation::Horizontal);
    }

    for (ScriptAsset* script : scripts)
    {
        Button* button = layout->AddChild<Button>(script->GetName());
        button->SetPreferredHeight(24.0f);

        const ComponentPickerSelection selection = {
            .kind = ComponentPickerItemKind::ScriptAsset,
            .scriptHandle = script->GetHandle()
        };

        button->GetOnClickRay().Register(
            this,
            [this, selection]
            {
                componentSelectedRay.Emit(selection);
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
