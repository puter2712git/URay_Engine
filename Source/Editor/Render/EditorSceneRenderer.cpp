#include "EditorSceneRenderer.h"

#include "Editor/Render/Visualizer/DirectionalLightVisualizer.h"
#include "Editor/Render/Visualizer/DecalVisualizer.h"
#include "Editor/Render/Visualizer/PointLightVisualizer.h"
#include "Editor/Render/Visualizer/SpotLightVisualizer.h"
#include "Editor/Selection/SelectionSystem.h"

#include "Engine/Component/Component.h"
#include "Engine/Component/Render/DecalComponent.h"
#include "Engine/Component/Render/Light/DirectionalLightComponent.h"
#include "Engine/Component/Render/Light/PointLightComponent.h"
#include "Engine/Component/Render/Light/SpotLightComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneSystem.h"
#include "Engine/Scene/Unit.h"

#include "Render/Scene/RenderScene.h"

namespace URay
{

EditorSceneRenderer::EditorSceneRenderer(Engine& engine, SelectionSystem& selectionSystem)
    : engine(engine), selectionSystem(selectionSystem) {}

EditorSceneRenderer::~EditorSceneRenderer() = default;

bool EditorSceneRenderer::Initialize()
{
    visualizerRegistry.Register<DirectionalLightComponent>([](EditorVisualContext& context, Unit& unit, Component& component)
                                                            { return std::make_unique<DirectionalLightVisualizer>(context, unit, component); });
    visualizerRegistry.Register<PointLightComponent>([this](EditorVisualContext& context, Unit& unit, Component& component)
                                                      { return std::make_unique<PointLightVisualizer>(context, unit, component, selectionSystem); });
    visualizerRegistry.Register<SpotLightComponent>([this](EditorVisualContext& context, Unit& unit, Component& component)
                                                     { return std::make_unique<SpotLightVisualizer>(context, unit, component, selectionSystem); });
    visualizerRegistry.Register<DecalComponent>([this](EditorVisualContext& context, Unit& unit, Component& component)
                                                 { return std::make_unique<DecalVisualizer>(context, unit, component, selectionSystem); });

    SceneSystem& sceneSystem = engine.GetSceneSystem();
    sceneSystem.GetUnitAddRay().Register(this, [this](Scene* scene, Unit* unit)
                                         { OnUnitAdded(scene, unit); });
    sceneSystem.GetUnitRemoveRay().Register(this, [this](Scene* scene, Unit* unit)
                                            { OnUnitRemoved(scene, unit); });
    sceneSystem.GetUnitWorldTransformUpdateRay().Register(this, [this](Scene* scene, Unit* unit)
                                                          { OnUnitTransformUpdated(scene, unit); });
    sceneSystem.GetComponentPropertyChangeRay().Register(this, [this](Scene* scene, Unit* unit, Component* component, const Property& property)
                                                         { OnComponentPropertyChanged(scene, unit, component, property); });

    return true;
}

void EditorSceneRenderer::Finalize()
{
    SceneSystem& sceneSystem = engine.GetSceneSystem();

    sceneSystem.GetComponentPropertyChangeRay().UnregisterAll(this);
    sceneSystem.GetUnitWorldTransformUpdateRay().UnregisterAll(this);
    sceneSystem.GetUnitRemoveRay().UnregisterAll(this);
    sceneSystem.GetUnitAddRay().UnregisterAll(this);
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
        if (visualizers.contains(component))
            continue;

        const EditorVisualizerRegistry::Constructor* constructor = visualizerRegistry.Find(component->GetClass());
        if (constructor)
        {
            std::unique_ptr<EditorComponentVisualizer> visualizer = (*constructor)(context, *unit, *component);
            visualizers.insert_or_assign(component, std::move(visualizer));
        }
    }
}

void EditorSceneRenderer::OnUnitRemoved(Scene*, Unit* unit)
{
    for (Component* component : unit->GetComponents())
    {
        visualizers.erase(component);
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
        const auto it = visualizers.find(component);
        if (it != visualizers.end())
        {
            it->second->OnUnitWorldTransformUpdated(context, *scene, *unit, *component);
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

    const auto it = visualizers.find(component);
    if (it != visualizers.end())
    {
        it->second->OnPropertyChanged(context, *scene, *unit, *component, property);
    }
}

} // namespace URay
