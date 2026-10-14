#pragma once

#include <Core/File/VirtualPath.h>
#include <Render/Rendering/RenderPipeline.h>

#include <memory>

namespace URay
{

class CameraComponent;
class Scene;

class WidgetSystem;
class EditorSettings;
class SelectionSystem;
class VisualSystem;

class InputActionMap;

class Editor
{
public:
    Editor();
    ~Editor();

public:
    bool Initialize();
    void Finalize();

    void Update();
    void PrepareRender();

    void StartGame();
    void StopGame();

    bool IsPlaying() const { return isPlaying; }

    void SetUseEditorCamera(bool value) { useEditorCamera = value; }
    bool UsingEditorCamera() const { return useEditorCamera; }

    Render::RenderRequest BuildRenderRequest() const;

    WidgetSystem& GetWidgetSystem() const { return *widgetSystem; }
    SelectionSystem& GetSelectionSystem() const { return *selectionSystem; }

private:
    CameraComponent& PrepareEditorScene();
    void ProcessInputActions();

    bool SaveCurrentScene();
    bool SaveSceneToPath(Scene& scene, const VirtualPath& path);
    void OpenSaveSceneDialog();

    void DeleteSelectedUnit();

private:
    CameraComponent* editorCamera = nullptr;

    std::unique_ptr<WidgetSystem> widgetSystem = nullptr;
    std::unique_ptr<SelectionSystem> selectionSystem = nullptr;
    std::unique_ptr<VisualSystem> visualSystem = nullptr;

    std::unique_ptr<EditorSettings> editorSettings = nullptr;

    bool isPlaying = false;
    bool useEditorCamera = false;

    std::unique_ptr<InputActionMap> inputActionMap = nullptr;
};

extern Editor* gEditor;

} // namespace URay
