#pragma once

#include "Editor/Widget/Widget.h"

namespace URay
{

class Editor;

class InspectorWidget final : public Widget
{
public:
    InspectorWidget(Editor& editor);
    ~InspectorWidget() override;

public:
    EventReply OnPointerDown(const PointerEvent& event) override;

protected:
    void OnDraw() override;

private:
    Editor& editor;
};

} // namespace URay
