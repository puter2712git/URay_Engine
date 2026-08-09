#include "MeshComponent.h"

#include "Engine/Asset/Material/MaterialManager.h"
#include "Engine/Asset/Mesh/MeshAsset.h"
#include "Engine/Asset/Mesh/MeshManager.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Object/Class/Class.h"
#include "Engine/Unit.h"

#include "Core/Math/Math.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"
#include "Render/DrawCommand/DrawCommandContext.h"

namespace URay
{

URAY_REGISTER_CLASS(MeshComponent)
URAY_REGISTER_COMPONENT(MeshComponent)

MeshComponent::MeshComponent()
{
    mesh = gEngine->GetMeshManager()->GetMesh("Box");
    materials.push_back(gEngine->GetMaterialManager()->GetOrCreate("Mesh"));
}

void MeshComponent::RegisterClass()
{
    Super::RegisterClass();

    StaticClass()->AddProperty({ PropertyType::Mesh, "Mesh", offsetof(MeshComponent, mesh), sizeof(MeshAsset*) });
}

void MeshComponent::SubmitCommand(DrawCommandBuilder& builder)
{
    if (!IsEnabled())
        return;

    TransformComponent* transform = GetOwner()->GetTransform();

    for (size_t i = 0; i < materials.size(); ++i)
    {
        MeshCommandContext context = {};
        context.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
        context.meshAsset = GetMesh();
        context.material = GetMaterial(i);

        builder.BuildFromMesh(context);
    }
}

bool MeshComponent::Pick(const Vector3& origin, const Vector3& direction, float& outDist) const
{
    Unit* owner = GetOwner();
    if (!owner)
        return false;

    TransformComponent* transform = owner->GetTransform();
    if (!transform)
        return false;

    const MeshAsset* mesh = GetMesh();
    if (!mesh)
        return false;

    const std::vector<VertexPNT>& vertices = mesh->GetVertices();
    const std::vector<uint32_t>& indices = mesh->GetIndices();

    bool isHit = false;
    float minDist = std::numeric_limits<float>::max();

    const Vector3 localOrigin = transform->InvTransformPoint(origin);
    const Vector3 localDir = transform->InvTransformVector(direction);

    for (size_t i = 0; i + 2 < indices.size(); i += 3)
    {
        const Vector3 p0 = vertices[indices[i]].position;
        const Vector3 p1 = vertices[indices[i + 1]].position;
        const Vector3 p2 = vertices[indices[i + 2]].position;

        float dist;
        bool hit = Math::IntersectLineTriangle(
            localOrigin, localDir,
            p0, p1, p2,
            dist);

        if (!hit)
            continue;

        if (dist < minDist)
        {
            minDist = dist;
            isHit = true;
        }
    }

    if (isHit)
    {
        outDist = minDist;
    }

    return isHit;
}

} // namespace URay
