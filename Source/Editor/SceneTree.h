#pragma once

namespace URay
{

class Engine;
class Editor;
class Unit;

class SceneTree
{
public:
    SceneTree(Editor& editor, Engine& engine);

public:
    void Draw() const;

private:
    void DrawUnit(Unit* unit, Unit* currSelectedUnit) const;

private:
    Engine& engine;
    Editor& editor;
};

} // namespace URay
