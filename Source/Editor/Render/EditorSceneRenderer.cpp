#include "EditorSceneRenderer.h"

#include "Editor/Render/DirectionalLightVisualizer.h"
#include "Editor/Render/PointLightVisualizer.h"

#include "Engine/Component/Component.h"
#include "Engine/Component/Render/Light/DirectionalLightComponent.h"
#include "Engine/Component/Render/Light/PointLightComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneSystem.h"
#include "Engine/Scene/Unit.h"

#include "Render/Scene/RenderScene.h"

namespace URay
{

EditorSceneRenderer::EditorSceneRenderer(Engine& engine)
    : engine(engine) {}

EditorSceneRenderer::~EditorSceneRenderer() = default;

bool EditorSceneRenderer::Initialize()
{
    visualizerRegistry.Register<DirectionalLightComponent>(std::make_unique<DirectionalLightVisualizer>());
    visualizerRegistry.Register<PointLightComponent>(std::make_unique<PointLightVisualizer>());

    SceneSystem& sceneSystem = engine.GetSceneSystem();
    unitAddHandle = sceneSystem.RegisterUnitAddCallback([this](Scene* scene, Unit* unit)
                                                        { OnUnitAdded(scene, unit); });
    unitRemoveHandle = sceneSystem.RegisterUnitRemoveCallback([this](Scene* scene, Unit* unit)
                                                              { OnUnitRemoved(scene, unit); });
    unitTransformUpdateHandle = sceneSystem.RegisterUnitWorldTransformUpdateCallback([this](Scene* scene, Unit* unit)
                                                                                     { OnUnitTransformUpdated(scene, unit); });
    componentPropertyChangedHandle = sceneSystem.RegisterComponentPropertyChangedCallback(
        [this](Scene* scene, Unit* unit, Component* component, const Property& property)
        {
            OnComponentPropertyChanged(scene, unit, component, property);
        });

    return true;
}

void EditorSceneRenderer::Finalize()
{
    SceneSystem& sceneSystem = engine.GetSceneSystem();

    sceneSystem.UnregisterComponentPropertyChangedCallback(componentPropertyChangedHandle);
    sceneSystem.UnregisterUnitWorldTransformUpdateCallback(unitTransformUpdateHandle);
    sceneSystem.UnregisterUnitRemoveCallback(unitRemoveHandle);
    sceneSystem.UnregisterUnitAddCallback(unitAddHandle);
}

void EditorSceneRenderer::OnUnitAdded(Scene* scene, Unit* unit)
{
    Scene* editorScene = engine.GetSceneSystem().GetSceneByType(SceneType::Editor);

    EditorVisualContext context = {
        .engine = engine,
        .renderScene = *editorScene->GetRenderScene()
    };

    for (Component* component : unit->GetComponents())
    {
        EditorComponentVisualizer* visualizer = visualizerRegistry.Find(component->GetClass());
        if (visualizer)
        {
            visualizer->OnAdded(context, *scene, *unit, *component);
        }
    }
}

void EditorSceneRenderer::OnUnitRemoved(Scene* scene, Unit* unit)
{
    Scene* editorScene = engine.GetSceneSystem().GetSceneByType(SceneType::Editor);

    EditorVisualContext context = {
        .engine = engine,
        .renderScene = *editorScene->GetRenderScene()
    };

    for (Component* component : unit->GetComponents())
    {
        EditorComponentVisualizer* visualizer = visualizerRegistry.Find(component->GetClass());
        if (visualizer)
        {
            visualizer->OnRemoved(context, *scene, *unit, *component);
        }
    }
}

void EditorSceneRenderer::OnUnitTransformUpdated(Scene* scene, Unit* unit)
{
    Scene* editorScene = engine.GetSceneSystem().GetSceneByType(SceneType::Editor);

    EditorVisualContext context = {
        .engine = engine,
        .renderScene = *editorScene->GetRenderScene()
    };

    for (Component* component : unit->GetComponents())
    {
        EditorComponentVisualizer* visualizer = visualizerRegistry.Find(component->GetClass());
        if (visualizer)
        {
            visualizer->OnUnitWorldTransformUpdated(context, *scene, *unit, *component);
        }
    }
}

void EditorSceneRenderer::OnComponentPropertyChanged(Scene* scene, Unit* unit, Component* component, const Property& property)
{
    Scene* editorScene = engine.GetSceneSystem().GetSceneByType(SceneType::Editor);

    EditorVisualContext context = {
        .engine = engine,
        .renderScene = *editorScene->GetRenderScene()
    };

    EditorComponentVisualizer* visualizer = visualizerRegistry.Find(component->GetClass());
    if (visualizer)
    {
        visualizer->OnPropertyChanged(context, *scene, *unit, *component, property);
    }
}

} // namespace URay
