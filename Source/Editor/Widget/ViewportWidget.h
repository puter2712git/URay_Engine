#pragma once

#include "Editor/Widget/Widget.h"

#include "Core/Math/Extent2D.h"

#include <optional>

namespace URay
{

namespace RHI
{
class Renderer;
}

class ViewportWidget : public Widget
{
public:
    ViewportWidget(RHI::Renderer& renderer);

public:
    EventReply OnPointerEnter() override;
    EventReply OnPointerLeave() override;
    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerMove(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

    EventReply OnKeyDown(const KeyEvent& event) override;
    EventReply OnKeyUp(const KeyEvent& event) override;

protected:
    void OnDraw() override;

private:
    std::optional<Vector2> WindowToRenderTarget(const Vector2& windowPosition) const;

private:
    RHI::Renderer& renderer;

    Rect imageRect = {};
    Extent2D targetExtent = {};
};

} // namespace URay
