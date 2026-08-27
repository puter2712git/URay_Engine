#include "RenderScene.h"

#include "Engine/Spatial/Octree.h"
#include "Render/Scene/Object/BoundedObject.h"
#include "Render/Scene/Object/RenderObject.h"
#include "Render/Scene/Object/ViewObject.h"

#include "Core/Math/AABB.h"

namespace URay::Render
{

RenderScene::RenderScene()
{
    octree = std::make_unique<URay::Octree>(AABB{
        .min = Vector3(-100.0f, -100.0f, -100.0f),
        .max = Vector3(100.0f, 100.0f, 100.0f),
    });
}

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

    if (BoundedObject* boundedObject =
            dynamic_cast<BoundedObject*>(renderObject))
    {
        octree->Insert(boundedObject);
    }
    else
    {
        unboundedObjects.push_back(renderObject);
    }

    objects.push_back(std::move(object));
}

} // namespace URay::Render
