#pragma once

#include "Editor/Render/EditorVisualizerRegistry.h"

#include <unordered_map>

namespace URay
{

namespace Render
{
class MeshObject;
}

class DirectionalLightVisualizer final : public IEditorComponentVisualizer
{
public:
    void OnAdded(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component) override;
    void OnRemoved(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component) override;
    void OnUnitWorldTransformUpdated(EditorVisualContext& context, Scene& scene, Unit& unit, Component& component) override;

private:
    std::unordered_map<Component*, Render::MeshObject*> meshObjects;
};

} // namespace URay
