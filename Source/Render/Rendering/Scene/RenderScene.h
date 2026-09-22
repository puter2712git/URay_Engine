#pragma once

#include <memory>
#include <vector>

namespace URay
{
class Component;
}

namespace URay::Render
{

class SceneSystem;
class RenderObject;
class ViewObject;

class RenderScene
{
public:
    RenderScene(SceneSystem& sceneSystem);
    ~RenderScene();

public:
    void Add(std::unique_ptr<RenderObject> object, Component* component);
    void Destroy(RenderObject* object);

    SceneSystem& GetSceneSystem() { return sceneSystem; }

    size_t GetObjectCount() const { return objects.size(); }
    RenderObject* GetObject(size_t index) const { return objects[index].get(); }

    ViewObject* GetView() const { return viewObjects.empty() ? nullptr : viewObjects[0]; }

private:
    SceneSystem& sceneSystem;

    std::vector<std::unique_ptr<RenderObject>> objects;
    std::vector<ViewObject*> viewObjects;
};

} // namespace URay::Render
