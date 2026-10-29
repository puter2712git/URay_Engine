#pragma once

#include <Core/File/VirtualPath.h>
#include <Engine/Object/ObjectId.h>
#include <Render/Rendering/RenderPipeline.h>

#include <memory>

namespace URay
{

class CameraComponent;
class Scene;
class Unit;
class EditorUI;
class EditorSettings;
class SelectionSystem;
class VisualSystem;
class InspectSystem;
class PlaySession;
class InputActionMap;
class ViewportController;
class PerformanceViewModel;
class SaveSceneDialog;
enum class DialogResult;
enum class UnitPopupSelection;
struct ComponentPickerSelection;

class Editor
{
public:
    Editor();
    ~Editor();

public:
    bool Initialize();
    void Finalize();

    void Update();
    void SetUseEditorCamera(bool value) { useEditorCamera = value; }
    bool UsingEditorCamera() const { return useEditorCamera; }

    Render::RenderRequest BuildRenderRequest() const;

    void LoadGameScene(const VirtualPath& scenePath);
    void OnUnitPopupSelected(UnitPopupSelection selection, bool createChild, ObjectId unitId);
    void OnComponentPopupSelected(ComponentPickerSelection selection, ObjectId unitId);

    EditorUI& GetEditorUI() const { return *editorUI; }
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
    void OnSaveSceneDialogClosed(DialogResult result, SaveSceneDialog* dialog, ObjectId sceneId);

    void DeleteSelectedUnits();

    void UpdatePerformanceViewModel();

    void OnActiveSelectionChanged(Unit* previousUnit, Unit* activeUnit);

private:
    std::unique_ptr<EditorUI> editorUI = nullptr;
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
