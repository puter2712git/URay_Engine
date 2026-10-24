#pragma once

#include <Core/File/VirtualPath.h>
#include <Core/Ray/EventRay.h>
#include <Engine/UI/Widget.h>

namespace URay
{

class Surface;
class Label;

class BreadcrumbItem final : public Widget
{
public:
    explicit BreadcrumbItem(VirtualPath path);

public:
    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

    EventRay<const VirtualPath&>& GetOnActivatedRay() { return onActivated; }

protected:
    Vector2 OnMeasure() override;

    void OnUpdate(float deltaTime) override;

private:
    VirtualPath path = {};

    Surface* surface = nullptr;
    Label* label = nullptr;

    EventRay<const VirtualPath&> onActivated;
};

} // namespace URay
