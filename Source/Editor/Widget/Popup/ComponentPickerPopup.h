#pragma once

#include <Core/Type/Ray.h>
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
    Ray<const ComponentPickerSelection&> onComponentSelected;
};

} // namespace URay
