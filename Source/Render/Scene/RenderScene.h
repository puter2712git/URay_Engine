#pragma once

#include <memory>
#include <vector>

namespace URay
{
class Octree;
}

namespace URay::Render
{

class RenderObject;
class ViewObject;

class RenderScene
{
public:
    RenderScene();
    ~RenderScene();

public:
    void Add(std::unique_ptr<RenderObject> object);

    size_t GetObjectCount() const { return objects.size(); }
    RenderObject* GetObject(size_t index) const { return objects[index].get(); }

    const std::vector<RenderObject*>& GetUnboundedObjects() const
    {
        return unboundedObjects;
    }

    URay::Octree* GetOctree() const { return octree.get(); }

    ViewObject* GetView() const { return viewObjects.empty() ? nullptr : viewObjects[0]; }

private:
    std::vector<std::unique_ptr<RenderObject>> objects;

    std::vector<ViewObject*> viewObjects;
    std::vector<RenderObject*> unboundedObjects;

    std::unique_ptr<URay::Octree> octree;
};

} // namespace URay::Render
