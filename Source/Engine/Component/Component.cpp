#include "Component.h"

#include <Engine/Object/Class/Class.h>
#include <Engine/Scene/Scene.h>
#include <Engine/Scene/SceneSystem.h>
#include <Engine/Scene/Unit.h>

namespace URay
{

URAY_REGISTER_CLASS(Component)

void Component::RegisterClass()
{
    StaticClass()->AddProperty({ .type = PropertyType::Bool,
                                 .name = "Enable",
                                 .offset = offsetof(Component, enabled),
                                 .size = sizeof(bool) });
}

void Component::Initialize()
{
    if (initialized || finalized)
        return;

    initialized = true;
    OnInitialize();
}

void Component::Load()
{
    if (!initialized || loaded || finalized)
        return;

    loaded = true;
    OnLoad();
}

void Component::Update(float deltaTime)
{
    if (!initialized || !enabled || finalized)
        return;

    if (!loaded && !CanUpdateWhenUnloaded())
        return;

    OnUpdate(deltaTime);
}

void Component::Unload()
{
    if (!loaded)
        return;

    OnUnload();
    loaded = false;
}

void Component::Finalize()
{
    if (finalized)
        return;

    Unload();

    if (initialized)
    {
        OnFinalize();
    }

    finalized = true;
}

void Component::NotifyPropertyChanged(const Property& property)
{
    Unit* unit = GetOwner();
    if (!unit)
        return;

    Scene* scene = unit->GetOwner();
    if (!scene)
        return;

    SceneSystem& sceneSystem = scene->GetSceneSystem();
    auto& onComponentPropertyChanged = sceneSystem.GetComponentPropertyChangeRay();
    onComponentPropertyChanged.Emit(scene, unit, this, property);
}

} // namespace URay
