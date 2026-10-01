#pragma once

#include "Editor/Widget/Widget.h"

namespace URay
{

class SelectionSystem;

class InspectorWidget final : public Widget
{
    URAY_TYPE(InspectorWidget, Widget)

public:
    InspectorWidget();
    ~InspectorWidget() override;

public:
    EventReply OnPointerDown(const PointerEvent& event) override;

protected:
    void OnDraw() override;

private:
    SelectionSystem& selectionSystem;
};

} // namespace URay
