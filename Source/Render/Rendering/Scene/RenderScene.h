#pragma once

#include <memory>
#include <vector>

namespace URay::Render
{

class SceneSystem;
class RenderObject;
class ViewObject;

class RenderScene
{
    friend class SceneSystem;

public:
    RenderScene(SceneSystem& sceneSystem);
    ~RenderScene();

public:
    SceneSystem& GetSceneSystem() { return sceneSystem; }

    size_t GetObjectCount() const { return objects.size(); }
    RenderObject* GetObject(size_t index) const { return objects[index].get(); }

    ViewObject* GetView() const { return viewObjects.empty() ? nullptr : viewObjects[0]; }

private:
    void Add(std::unique_ptr<RenderObject> object);
    void Destroy(RenderObject* object);

private:
    SceneSystem& sceneSystem;

    std::vector<std::unique_ptr<RenderObject>> objects;
    std::vector<ViewObject*> viewObjects;
};

} // namespace URay::Render
