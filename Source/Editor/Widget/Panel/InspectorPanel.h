#pragma once

#include "Editor/Widget/Panel/Panel.h"

namespace URay
{

class SelectionSystem;

class InspectorPanel final : public Panel
{
    URAY_TYPE(InspectorPanel, Panel)

public:
    InspectorPanel();
    ~InspectorPanel() override;

public:
    EventReply OnPointerDown(const PointerEvent& event) override;

protected:
    void OnDraw() override;

private:
    SelectionSystem& selectionSystem;
};

} // namespace URay
