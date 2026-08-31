#pragma once

#include <memory>

namespace URay
{

class Engine;
class CameraComponent;
class Unit;

namespace Render
{
class Renderer;
}

class Widget;
class ViewportWidget;
class UIInputRouter;
class EditorSettings;
class SelectionSystem;
class WidgetDrawer;

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

private:
    CameraComponent& PrepareEditorScene();

private:
    Engine& engine;

    CameraComponent* editorCamera = nullptr;

    std::unique_ptr<SelectionSystem> selectionSystem = nullptr;

    std::unique_ptr<Widget> mainMenuBarWidget = nullptr;
    std::unique_ptr<Widget> rootWidget = nullptr;

    ViewportWidget* viewportWidget = nullptr;

    std::unique_ptr<WidgetDrawer> widgetDrawer = nullptr;

    std::unique_ptr<UIInputRouter> inputRouter = nullptr;
    std::unique_ptr<EditorSettings> editorSettings = nullptr;
};

} // namespace URay
