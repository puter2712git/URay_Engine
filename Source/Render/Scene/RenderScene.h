#pragma once

#include <memory>
#include <vector>

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

    ViewObject* GetView() const { return viewObjects.empty() ? nullptr : viewObjects[0]; }

private:
    std::vector<std::unique_ptr<RenderObject>> objects;

    std::vector<ViewObject*> viewObjects;
};

} // namespace URay::Render
