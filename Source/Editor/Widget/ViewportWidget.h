#pragma once

#include "Editor/Widget/Widget.h"

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
    RHI::Renderer& renderer;
};

} // namespace URay
