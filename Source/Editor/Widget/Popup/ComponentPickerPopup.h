#pragma once
#include <Core/Type/Types.h>

#include <Core/Type/Ray/Ray.h>
#include <Editor/Widget/Popup/PopupMenu.h>

namespace URay
{

class Unit;

struct ComponentPickerSelection
{
    String componentTypeName;
};

class ComponentPickerPopup final : public PopupMenu
{
public:
    explicit ComponentPickerPopup(const Unit& unit);

public:
    Ray<const ComponentPickerSelection&> onComponentSelected;
};

} // namespace URay
