#pragma once

#include <Core/Ray/EventRay.h>
#include <Editor/Widget/Widget.h>

#include <string>

namespace URay
{

class Unit;
class Surface;
class VerticalLayout;
class Button;

class ComponentPickerPopup final : public Widget
{
public:
    explicit ComponentPickerPopup(const Unit& unit);

public:
    void Arrange(const Rect& rect) override;

    EventRay<const std::string&>& GetComponentSelectedRay() { return componentSelectedRay; }

protected:
    Vector2 OnMeasure() override;

private:
    Surface* surface = nullptr;
    VerticalLayout* layout = nullptr;

    EventRay<const std::string&> componentSelectedRay;
};

} // namespace URay
