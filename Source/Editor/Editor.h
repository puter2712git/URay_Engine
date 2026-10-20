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
class InspectSystem;
class PlaySession;
class InputActionMap;
class ViewportController;
class PerformanceViewModel;

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

    void SetUseEditorCamera(bool value) { useEditorCamera = value; }
    bool UsingEditorCamera() const { return useEditorCamera; }

    Render::RenderRequest BuildRenderRequest() const;

    void LoadGameScene(const VirtualPath& scenePath);

    WidgetSystem& GetWidgetSystem() const { return *widgetSystem; }
    SelectionSystem& GetSelectionSystem() const { return *selectionSystem; }
    InspectSystem& GetInspectSystem() const { return *inspectSystem; }

    PlaySession& GetPlaySession() const { return *playSession; }

    const PerformanceViewModel& GetPerformanceViewModel() const { return *performanceViewModel; }

private:
    void PrepareGrid();
    void ProcessInputActions();

    bool SaveCurrentScene();
    bool SaveSceneToPath(Scene& scene, const VirtualPath& path);
    void OpenSaveSceneDialog();

    void DeleteSelectedUnits();

    void UpdatePerformanceViewModel();

private:
    std::unique_ptr<WidgetSystem> widgetSystem = nullptr;
    std::unique_ptr<ViewportController> viewportController = nullptr;
    std::unique_ptr<SelectionSystem> selectionSystem = nullptr;
    std::unique_ptr<VisualSystem> visualSystem = nullptr;
    std::unique_ptr<InspectSystem> inspectSystem = nullptr;

    std::unique_ptr<EditorSettings> editorSettings = nullptr;

    std::unique_ptr<PlaySession> playSession = nullptr;
    bool useEditorCamera = false;

    std::unique_ptr<InputActionMap> inputActionMap = nullptr;

    std::unique_ptr<PerformanceViewModel> performanceViewModel = nullptr;
    uint64 lastProfileFrameNumber = 0;
};

extern Editor* gEditor;

} // namespace URay
