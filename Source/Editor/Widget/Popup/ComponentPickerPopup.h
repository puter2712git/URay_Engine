#pragma once

#include <Core/Ray/EventRay.h>
#include <Editor/Widget/Widget.h>
#include <Engine/Asset/Asset.h>

#include <string>

namespace URay
{

class Unit;
class Surface;
class VerticalLayout;
class Button;

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

class ComponentPickerPopup final : public Widget
{
public:
    explicit ComponentPickerPopup(const Unit& unit);

public:
    void Arrange(const Rect& rect) override;

    EventRay<const ComponentPickerSelection&>& GetComponentSelectedRay() { return componentSelectedRay; }

protected:
    Vector2 OnMeasure() override;

private:
    Surface* surface = nullptr;
    VerticalLayout* layout = nullptr;

    EventRay<const ComponentPickerSelection&> componentSelectedRay;
};

} // namespace URay
