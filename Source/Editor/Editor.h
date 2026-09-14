#pragma once

#include "Render/RenderPipeline.h"

#include <memory>

namespace URay
{

class CameraComponent;

class WidgetSystem;
class EditorSettings;
class SelectionSystem;
class VisualSystem;

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

private:
    CameraComponent* editorCamera = nullptr;

    std::unique_ptr<WidgetSystem> widgetSystem = nullptr;
    std::unique_ptr<SelectionSystem> selectionSystem = nullptr;
    std::unique_ptr<VisualSystem> visualSystem = nullptr;

    std::unique_ptr<EditorSettings> editorSettings = nullptr;

    bool isPlaying = false;
    bool useEditorCamera = false;
};

extern Editor* gEditor;

} // namespace URay
