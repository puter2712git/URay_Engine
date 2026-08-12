#pragma once

namespace URay
{

class Engine;
class Renderer;
class Unit;

class EditorPicker;
class MainMenuBar;
class SceneTree;
class EditorConsole;
class FilesystemWidget;

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

    void UpdateHover();
    void UpdatePick();

private:
    Engine& engine;

    EditorPicker* picker = nullptr;
    Unit* selectedUnit = nullptr;

    MainMenuBar* mainMenuBar = nullptr;
    SceneTree* sceneTree = nullptr;
    EditorConsole* console = nullptr;
    FilesystemWidget* filesystemWidget = nullptr;
};

} // namespace URay
