#pragma once
#include <Core/Type/TypeAlias.h>

#include <Editor/Widget/Popup/PopupMenu.h>

namespace URay
{

class Unit;

struct ComponentPickerSelection
{
    URAY_TYPE(ComponentPickerSelection)

    String componentTypeName;
};

class ComponentPickerPopup final : public PopupMenu
{
public:
    explicit ComponentPickerPopup(const Unit& unit);

private:
    void OnMenuItemSelected(int32 id);
    std::vector<String> componentNames;

};

} // namespace URay
