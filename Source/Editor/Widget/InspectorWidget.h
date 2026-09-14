#pragma once

#include "Editor/Widget/Widget.h"

namespace URay
{

class SelectionSystem;

class InspectorWidget final : public Widget
{
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
