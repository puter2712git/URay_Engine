#include "SelectionSystem.h"

#include "Core/Math/AABB.h"

#include "Engine/Component/Render/DecalComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Scene/Unit.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"

namespace URay
{

SelectionSystem::SelectionSystem() = default;

SelectionSystem::~SelectionSystem() = default;

void SelectionSystem::PrepareRender(Render::DrawCommandBuilder& builder)
{
    if (!selectedUnit)
        return;

    TransformComponent* transform = selectedUnit->GetTransform();
    Matrix worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;

    for (Component* comp : selectedUnit->GetComponents())
    {
        if (DecalComponent* decal = Cast<DecalComponent>(comp))
        {
            const Vector3 decalExtent = decal->GetExtent();
            const AABB localBounds = {
                .min = Vector3(-decalExtent.x, -decalExtent.y, -decalExtent.z),
                .max = Vector3(decalExtent.x, decalExtent.y, decalExtent.z)
            };

            builder.BuildOBB(localBounds, worldMatrix);
        }
    }
}

void SelectionSystem::SelectUnit(Unit* unit)
{
    selectedUnit = unit;
    onSelectRay.Emit(selectedUnit);
}

RayHandle SelectionSystem::RegisterOnSelected(EventRay<Unit*>::Callback callback)
{
    RayHandle handle = onSelectRay.Register(callback);
    return handle;
}

void SelectionSystem::UnregisterOnSelected(RayHandle handle)
{
    onSelectRay.Unregister(handle);
}

} // namespace URay
