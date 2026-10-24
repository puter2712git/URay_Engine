#pragma once

#include <Engine/UI/Widget.h>

#include <Core/Type/Types.h>

namespace URay
{

class Unit;
class HorizontalLayout;
class Label;
class Surface;
class SelectionSystem;

class SceneTreeNode final : public Widget
{
public:
    SceneTreeNode();

public:
    void Bind(Unit& unit, uint32 depth);
    void Unbind();

    void Arrange(const Rect& rect) override;

    EventReply OnPointerDown(const PointerEvent& event) override;

protected:
    void OnUpdate(float deltaTime) override;

private:
    SelectionSystem* selectionSystem = nullptr;

    Unit* unit = nullptr;
    uint32 depth = 0;

    Surface* surface = nullptr;
    HorizontalLayout* layout = nullptr;
    Label* label = nullptr;
};

} // namespace URay
