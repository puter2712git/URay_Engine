#pragma once

namespace URay
{

class Engine;
class Renderer;
class Unit;

class MainMenuBar;
class SceneTree;

class Editor
{
public:
    Editor(Engine& engine);
    ~Editor() = default;

public:
    bool Initialize();
    void Finalize();

    void Update();
    void PrepareRender();

    Unit* GetSelectedUnit() const { return selectedUnit; }
    void SelectUnit(Unit* unit);

private:
    void ShowStatus() const;
    void ShowInspector() const;

private:
    Engine& engine;

    Unit* selectedUnit = nullptr;

    MainMenuBar* mainMenuBar = nullptr;
    SceneTree* sceneTree = nullptr;
};

} // namespace URay
