#pragma once

namespace URay
{

class Engine;
class Renderer;
class Unit;

class SceneTree;

class Editor
{
public:
    Editor(Engine& engine, Renderer& renderer);
    ~Editor() = default;

public:
    bool Initialize();
    void Finalize();

    void Update(float deltaTime);
    void Render();

    Unit* GetSelectedUnit() const { return selectedUnit; }
    void SelectUnit(Unit* unit);

private:
    void ShowStatus() const;
    void ShowInspector() const;

private:
    Engine& engine;
    Renderer& renderer;

    Unit* selectedUnit = nullptr;

    SceneTree* sceneTree = nullptr;
};

} // namespace URay
