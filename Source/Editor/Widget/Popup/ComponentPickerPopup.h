#pragma once

#include <Core/Ray/EventRay.h>
#include <Editor/Widget/Popup/PopupMenu.h>
#include <string>

namespace URay
{

class Unit;

struct ComponentPickerSelection
{
    std::string componentTypeName;
};

class ComponentPickerPopup final : public PopupMenu
{
public:
    explicit ComponentPickerPopup(const Unit& unit);

public:
    EventRay<const ComponentPickerSelection&>& GetComponentSelectedRay() { return onComponentSelected; }

private:
    EventRay<const ComponentPickerSelection&> onComponentSelected;
};

} // namespace URay
