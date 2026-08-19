#pragma once

#include "Editor/Input/UIInputRouter.h"

#include <memory>

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

class Widget;
class EditorPicker;

class Editor
{
public:
    Editor(Engine& engine);
    ~Editor();

public:
    bool Initialize();
    void Finalize();

    void Update();
    void PrepareRender();

    Unit* GetSelectedUnit() const { return selectedUnit; }
    void SelectUnit(Unit* unit);

private:
    void PrepareEditorScene();

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

    std::unique_ptr<Widget> mainMenuBarWidget = nullptr;
    std::unique_ptr<Widget> rootWidget = nullptr;

    UIInputRouter inputRouter;
};

} // namespace URay
