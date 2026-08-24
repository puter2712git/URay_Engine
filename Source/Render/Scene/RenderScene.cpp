#include "RenderScene.h"

#include "Render/Scene/Object/RenderObject.h"
#include "Render/Scene/Object/ViewObject.h"

namespace URay::Render
{

RenderScene::RenderScene() = default;

RenderScene::~RenderScene() = default;

void RenderScene::Add(std::unique_ptr<RenderObject> object)
{
    if (ViewObject* viewObj = dynamic_cast<ViewObject*>(object.get()))
    {
        viewObjects.push_back(viewObj);
    }

    objects.push_back(std::move(object));
}

} // namespace URay::Render
