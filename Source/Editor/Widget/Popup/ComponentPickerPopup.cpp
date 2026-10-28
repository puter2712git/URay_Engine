#include "ComponentPickerPopup.h"
#include <Core/Type/TypeAlias.h>

#include <Engine/Component/ComponentFactory.h>

#include <algorithm>
#include <vector>

namespace URay
{

ComponentPickerPopup::ComponentPickerPopup(const Unit& unit)
{
    URAY_DECLARE_RAY(ComponentSelected, ComponentPickerSelection);
    Register("ItemSelected", this, &ComponentPickerPopup::OnMenuItemSelected);

    for (const auto& [name, constructor] : ComponentFactory::GetRegisteredComponents())
    {
        componentNames.push_back(name);
    }

    std::sort(componentNames.begin(), componentNames.end());

    for (const String& typeName : componentNames)
    {
        const int32 id = static_cast<int32>(&typeName - componentNames.data());
        AddMenuItem(typeName, id);
    }
}

void ComponentPickerPopup::OnMenuItemSelected(int32 id)
{
    if (id < 0 || static_cast<size_t>(id) >= componentNames.size())
        return;

    const ComponentPickerSelection selection = { .componentTypeName = componentNames[static_cast<size_t>(id)] };
    Emit("ComponentSelected", selection);
}

} // namespace URay
