#pragma once

#include "Editor/Widget/Panel/Panel.h"

namespace URay
{

class SelectionSystem;

class InspectorWidget final : public Panel
{
    URAY_TYPE(InspectorWidget, Panel)

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
