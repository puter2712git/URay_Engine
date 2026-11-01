#pragma once

#include <Core/Type/TypeAlias.h>
#include <Core/File/VirtualPath.h>
#include <Engine/Asset/AssetHandle.h>
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
class AssetEditorSystem;
class Window;
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
    void BuildAdditionalWindowRenderRequest(
        Window& window, Render::RenderRequest& request) const;

    void LoadGameScene(const VirtualPath& scenePath);
    void OnUnitPopupSelected(UnitPopupSelection selection, bool createChild, ObjectId unitId);
    void OnComponentPopupSelected(ComponentPickerSelection selection, ObjectId unitId);

    EditorUI& GetEditorUI() const { return *editorUI; }
    SelectionSystem& GetSelectionSystem() const { return *selectionSystem; }
    InspectSystem& GetInspectSystem() const { return *inspectSystem; }

    PlaySession& GetPlaySession() const { return *playSession; }

    const PerformanceViewModel& GetPerformanceViewModel() const { return *performanceViewModel; }

    AssetEditorSystem& GetAssetEditorSystem() const { return *assetEditorSystem; }

private:
    void OpenMeshPreview(const AssetHandle& meshHandle);
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
    UniquePtr<EditorUI> editorUI = nullptr;
    UniquePtr<ViewportController> viewportController = nullptr;
    UniquePtr<SelectionSystem> selectionSystem = nullptr;
    UniquePtr<VisualSystem> visualSystem = nullptr;
    UniquePtr<InspectSystem> inspectSystem = nullptr;

    UniquePtr<EditorSettings> editorSettings = nullptr;

    UniquePtr<PlaySession> playSession = nullptr;
    bool useEditorCamera = false;

    UniquePtr<InputActionMap> inputActionMap = nullptr;

    UniquePtr<PerformanceViewModel> performanceViewModel = nullptr;
    uint64 lastProfileFrameNumber = 0;

    UniquePtr<AssetEditorSystem> assetEditorSystem = nullptr;
};

extern Editor* gEditor;

} // namespace URay
