#include "VisualSystem.h"

#include "Editor/Selection/SelectionSystem.h"
#include "Editor/Visual/Visualizer/DecalVisualizer.h"
#include "Editor/Visual/Visualizer/DirectionalLightVisualizer.h"
#include "Editor/Visual/Visualizer/PointLightVisualizer.h"
#include "Editor/Visual/Visualizer/SpotLightVisualizer.h"

#include "Engine/Component/Component.h"
#include "Engine/Component/Render/DecalComponent.h"
#include "Engine/Component/Render/Light/DirectionalLightComponent.h"
#include "Engine/Component/Render/Light/PointLightComponent.h"
#include "Engine/Component/Render/Light/SpotLightComponent.h"
#include "Engine/Engine.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneSystem.h"
#include "Engine/Scene/Unit.h"

#include "Render/RenderSystem.h"
#include "Render/Rendering/Scene/RenderScene.h"
#include "Render/Rendering/Scene/SceneSystem.h"

namespace URay
{

VisualSystem::VisualSystem(Engine& engine, SelectionSystem& selectionSystem)
    : engine(engine), selectionSystem(selectionSystem) {}

VisualSystem::~VisualSystem() = default;

bool VisualSystem::Initialize()
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
    sceneSystem.GetUnitWorldTransformUpdateRay().Register(this, [this](Scene* scene, Unit* unit)
                                                          { OnUnitTransformUpdated(scene, unit); });
    sceneSystem.GetComponentAddRay().Register(this, [this](Scene* scene, Unit* unit, Component* component)
                                              { OnComponentAdded(scene, unit, component); });
    sceneSystem.GetComponentDestroyRay().Register(this, [this](Scene* scene, Unit* unit, Component* component)
                                                  { OnComponentDestroyed(scene, unit, component); });
    sceneSystem.GetComponentPropertyChangeRay().Register(this, [this](Scene* scene, Unit* unit, Component* component, const Property& property)
                                                         { OnComponentPropertyChanged(scene, unit, component, property); });

    return true;
}

void VisualSystem::Finalize()
{
    SceneSystem& sceneSystem = engine.GetSceneSystem();

    sceneSystem.GetComponentPropertyChangeRay().UnregisterAll(this);
    sceneSystem.GetComponentDestroyRay().UnregisterAll(this);
    sceneSystem.GetComponentAddRay().UnregisterAll(this);
    sceneSystem.GetUnitWorldTransformUpdateRay().UnregisterAll(this);
}

void VisualSystem::OnUnitTransformUpdated(Scene* scene, Unit* unit)
{
    Render::SceneSystem& sceneSystem = gEngine->GetRenderSystem().GetSceneSystem();
    Render::RenderScene* renderScene = sceneSystem.GetRenderScene(scene);

    EditorVisualContext context = {
        .engine = engine,
        .renderScene = *renderScene
    };

    for (const auto& component : unit->GetComponents())
    {
        const auto it = visualizers.find(component.get());
        if (it != visualizers.end())
        {
            it->second->OnUnitWorldTransformUpdated(context, *scene, *unit, *component);
        }
    }
}

void VisualSystem::OnComponentAdded(Scene* scene, Unit* unit, Component* component)
{
    Render::SceneSystem& sceneSystem = gEngine->GetRenderSystem().GetSceneSystem();
    Render::RenderScene* renderScene = sceneSystem.GetRenderScene(scene);

    if (!renderScene)
        return;

    EditorVisualContext context = {
        .engine = engine,
        .renderScene = *renderScene
    };

    if (visualizers.contains(component))
        return;

    const EditorVisualizerRegistry::Constructor* constructor = visualizerRegistry.Find(component->GetClass());
    if (constructor)
    {
        std::unique_ptr<EditorComponentVisualizer> visualizer = (*constructor)(context, *unit, *component);
        visualizers.insert_or_assign(component, std::move(visualizer));
    }
}

void VisualSystem::OnComponentDestroyed(Scene* scene, Unit* unit, Component* component)
{
    visualizers.erase(component);
}

void VisualSystem::OnComponentPropertyChanged(Scene* scene, Unit* unit, Component* component, const Property& property)
{
    Render::SceneSystem& sceneSystem = gEngine->GetRenderSystem().GetSceneSystem();
    Render::RenderScene* renderScene = sceneSystem.GetRenderScene(scene);

    EditorVisualContext context = {
        .engine = engine,
        .renderScene = *renderScene
    };

    const auto it = visualizers.find(component);
    if (it != visualizers.end())
    {
        it->second->OnPropertyChanged(context, *scene, *unit, *component, property);
    }
}

} // namespace URay
