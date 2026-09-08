#pragma once

#include "Engine/Ray/EventRay.h"
#include "Engine/Scene/Scene.h"

#include <memory>
#include <vector>

namespace URay
{

class Component;

class SceneSystem
{
public:
    bool Initialize();
    void Finalize();

public:
    void Update(float deltaTime);

    std::unique_ptr<Scene> CreateScene(SceneType type, const VirtualPath& filePath);

    void SwitchScene(std::unique_ptr<Scene> scene);

    void LoadScene(std::unique_ptr<Scene> scene);
    void UnloadScene(SceneType type);

    Scene* GetSceneByType(SceneType type) const;

    const std::vector<std::unique_ptr<Scene>>& GetScenes() const { return scenes; }

    RayHandle RegisterUnitAddCallback(EventRay<Scene*, Unit*>::Callback callback) { return unitAddRay.Register(callback); }
    void UnregisterUnitAddCallback(RayHandle handle) { unitAddRay.Unregister(handle); }

    RayHandle RegisterUnitRemoveCallback(EventRay<Scene*, Unit*>::Callback callback) { return unitRemoveRay.Register(callback); }
    void UnregisterUnitRemoveCallback(RayHandle handle) { unitRemoveRay.Unregister(handle); }

    RayHandle RegisterUnitWorldTransformUpdateCallback(EventRay<Scene*, Unit*>::Callback callback) { return unitWorldTransformUpdateRay.Register(callback); }
    void UnregisterUnitWorldTransformUpdateCallback(RayHandle handle) { unitWorldTransformUpdateRay.Unregister(handle); }

    void EmitUnitAddRay(Scene* scene, Unit* unit) { unitAddRay.Emit(scene, unit); }
    void EmitUnitRemoveRay(Scene* scene, Unit* unit) { unitRemoveRay.Emit(scene, unit); }
    void EmitUnitWorldTransformUpdateRay(Scene* scene, Unit* unit) { unitWorldTransformUpdateRay.Emit(scene, unit); }

private:
    std::vector<std::unique_ptr<Scene>> scenes;

    EventRay<Scene*, Unit*> unitAddRay;
    EventRay<Scene*, Unit*> unitRemoveRay;
    EventRay<Scene*, Unit*> unitWorldTransformUpdateRay;
};

} // namespace URay
