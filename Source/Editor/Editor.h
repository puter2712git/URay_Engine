#pragma once

namespace URay
{

class Engine;
class CameraComponent;
class GizmoController;

namespace RHI
{
class Renderer;
}
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
    void EndRender();

    Unit* GetSelectedUnit() const { return selectedUnit; }
    void SelectUnit(Unit* unit);

private:
    void PrepareEditorScene();

    void ShowStatus() const;
    void ShowInspector() const;

    void UpdateCameraMovement(float deltaTime);
    void UpdateCameraRotation(float deltaTime);

    void UpdateHover();
    void UpdatePick();

private:
    Engine& engine;

    CameraComponent* camera = nullptr;
    GizmoController* gizmo = nullptr;

    EditorPicker* picker = nullptr;
    Unit* selectedUnit = nullptr;

    MainMenuBar* mainMenuBar = nullptr;
    SceneTree* sceneTree = nullptr;
    EditorConsole* console = nullptr;
    FilesystemWidget* filesystemWidget = nullptr;
};

} // namespace URay
