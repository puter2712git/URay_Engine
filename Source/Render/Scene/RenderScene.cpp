#include "RenderScene.h"

#include "Engine/Spatial/Octree.h"
#include "Render/Scene/Object/BoundedObject.h"
#include "Render/Scene/Object/RenderObject.h"
#include "Render/Scene/Object/ViewObject.h"

#include "Core/Math/AABB.h"

namespace URay::Render
{

RenderScene::RenderScene() = default;

RenderScene::~RenderScene() = default;

void RenderScene::Add(std::unique_ptr<RenderObject> object)
{
    if (!object)
        return;

    RenderObject* renderObject = object.get();

    if (ViewObject* viewObj = dynamic_cast<ViewObject*>(object.get()))
    {
        viewObjects.push_back(viewObj);
    }

    objects.push_back(std::move(object));
}

void RenderScene::Destroy(RenderObject* object)
{
    if (!object)
        return;

    std::erase_if(objects, [&](std::unique_ptr<RenderObject>& robj)
                  { return object == robj.get(); });
}

} // namespace URay::Render
