#pragma once

namespace URay
{

class Unit;

class SceneTree
{
public:
    void Draw() const;

private:
    void DrawUnit(Unit* unit, Unit* currSelectedUnit) const;
};

} // namespace URay
