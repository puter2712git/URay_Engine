#pragma once

#include "Editor/Widget/Widget.h"

#include "Core/Math/Extent2D.h"

#include <functional>
#include <memory>
#include <optional>

namespace URay
{

class CameraComponent;
class Engine;
class Unit;

class GizmoController;
class EditorPicker;

namespace RHI
{
class Renderer;
}

class ViewportWidget : public Widget
{
public:
    ViewportWidget(RHI::Renderer& renderer, CameraComponent& camera, Engine& engine, std::function<void(Unit*)> onSelectUnitFunc);
    ~ViewportWidget() override;

public:
    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerMove(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

    EventReply OnKeyDown(const KeyEvent& event) override;
    EventReply OnKeyUp(const KeyEvent& event) override;

    void SetSelectedUnit(Unit* unit);

protected:
    void OnDraw() override;

private:
    std::optional<Vector2> WindowToRenderTarget(const Vector2& windowPosition) const;

private:
    RHI::Renderer& renderer;

    CameraComponent& camera;
    std::unique_ptr<GizmoController> gizmo = nullptr;
    std::unique_ptr<EditorPicker> picker = nullptr;

    std::function<void(Unit*)> onSelectUnit;

    Rect imageRect = {};
    Extent2D targetExtent = {};
};

} // namespace URay
