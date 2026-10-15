#pragma once

#include <Core/Ray/EventRay.h>
#include <Editor/Widget/Popup/PopupMenu.h>
#include <Engine/Asset/AssetHandle.h>

#include <string>

namespace URay
{

class Unit;

enum class ComponentPickerItemKind
{
    NativeComponent,
    ScriptAsset
};

struct ComponentPickerSelection
{
    ComponentPickerItemKind kind = ComponentPickerItemKind::NativeComponent;
    std::string componentTypeName;
    AssetHandle scriptHandle = {};
};

class ComponentPickerPopup final : public PopupMenu
{
public:
    explicit ComponentPickerPopup(const Unit& unit);

public:
    EventRay<const ComponentPickerSelection&>& GetComponentSelectedRay() { return componentSelectedRay; }

private:
    EventRay<const ComponentPickerSelection&> componentSelectedRay;
};

} // namespace URay
