#include "ComponentPickerPopup.h"

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
        const ComponentPickerSelection selection = {
            .kind = ComponentPickerItemKind::NativeComponent,
            .componentTypeName = typeName
        };

        AddMenuItem(typeName, [this, selection]
                    { componentSelectedRay.Emit(selection); });
    }

    AssetDatabase& database = gEngine->GetAssetSystem().GetDatabase();

    std::vector<ScriptAsset*> scripts = database.GetAssets<ScriptAsset>();

    std::sort(scripts.begin(), scripts.end(),
              [](const ScriptAsset* lhs, const ScriptAsset* rhs)
              { return lhs->GetName() < rhs->GetName(); });

    if (!componentNames.empty() && !scripts.empty())
    {
        AddMenuSeparator();
    }

    for (ScriptAsset* script : scripts)
    {
        const ComponentPickerSelection selection = {
            .kind = ComponentPickerItemKind::ScriptAsset,
            .scriptHandle = script->GetHandle()
        };

        AddMenuItem(script->GetName(), [this, selection]
                    { componentSelectedRay.Emit(selection); });
    }
}

} // namespace URay
