#include "ComponentPickerPopup.h"

#include <Engine/Component/ComponentFactory.h>

#include <algorithm>
#include <vector>

namespace URay
{

ComponentPickerPopup::ComponentPickerPopup(const Unit& unit)
{
    std::vector<std::string> componentNames;

    for (const auto& [name, constructor] : ComponentFactory::GetRegisteredComponents())
    {
        componentNames.push_back(name);
    }

    std::sort(componentNames.begin(), componentNames.end());

    for (const std::string& typeName : componentNames)
    {
        const ComponentPickerSelection selection = { .componentTypeName = typeName };

        AddMenuItem(typeName, [this, selection]
                    { onComponentSelected.Emit(selection); });
    }
}

} // namespace URay
