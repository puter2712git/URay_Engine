#pragma once

#include <memory>
#include <vector>

namespace URay::RHI
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

private:
    std::vector<std::unique_ptr<RenderObject>> objects;

    std::vector<ViewObject*> viewObjects;
};

} // namespace URay::RHI
