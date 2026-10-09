#pragma once

#include <Editor/Widget/Widget.h>

#include <Core/Type/Types.h>

namespace URay
{

class Unit;
class HorizontalLayout;
class Label;

class SceneTreeNode final : public Widget
{
public:
    SceneTreeNode(Unit& unit, uint32 depth);

public:
    void Arrange(const Rect& rect) override;

protected:
    void OnUpdate(float deltaTime) override;

private:
    Unit& unit;
    uint32 depth = 0;

    HorizontalLayout* layout = nullptr;
    Label* label = nullptr;
};

} // namespace URay
