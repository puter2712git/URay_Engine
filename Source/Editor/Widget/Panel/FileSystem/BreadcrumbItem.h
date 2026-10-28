#pragma once

#include <Core/File/VirtualPath.h>
#include <Core/Type/Ray/RayEmitter.h>
#include <Engine/UI/Widget/Widget.h>

namespace URay
{

class Surface;
class Label;

class BreadcrumbItem final : public Widget, public RayEmitter
{
public:
    explicit BreadcrumbItem(VirtualPath path);

public:
    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

protected:
    Vector2 OnMeasure() override;

    void OnUpdate(float deltaTime) override;

private:
    VirtualPath path = {};

    Surface* surface = nullptr;
    Label* label = nullptr;

};

} // namespace URay
