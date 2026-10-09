#pragma once

#include <Editor/Widget/Widget.h>

#include <Core/Ray/EventRay.h>

namespace URay
{

class Checkbox : public Widget
{
public:
    Checkbox();

public:
    EventReply OnPointerDown(const PointerEvent& event) override;
    EventReply OnPointerUp(const PointerEvent& event) override;

    bool IsChecked() const { return checked; }

    EventRay<bool>& GetValueChangeRay() { return valueChangeRay; }

protected:
    void OnPaint(Render::UIDrawContext& context) override;

private:
    bool checked = false;

    EventRay<bool> valueChangeRay;
};

} // namespace URay
