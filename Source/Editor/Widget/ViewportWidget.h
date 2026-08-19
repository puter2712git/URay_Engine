#pragma once

#include "Editor/Widget/Widget.h"

namespace URay
{

class ViewportWidget : public Widget
{
public:
    EventReply OnPointerEnter() override;
    EventReply OnPointerLeave() override;
    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerMove(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

protected:
    void OnDraw() override;
};

} // namespace URay
