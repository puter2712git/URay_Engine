#include "Component.h"

#include "Engine/Object/Class/Class.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneSystem.h"
#include "Engine/Scene/Unit.h"

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

void Component::Update(float deltaTime)
{
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
    auto& ray = sceneSystem.GetComponentPropertyChangeRay();
    ray.Emit(scene, unit, this, property);
}

} // namespace URay
