#include "MeshPickObject.h"

#include "Engine/Asset/Mesh/Mesh.h"
#include "Engine/Component/Render/MeshComponent.h"
#include "Engine/Component/TransformComponent.h"
#include "Engine/Scene/Unit.h"

#include "Core/Math/Math.h"

#include "Render/Vertex.h"

namespace URay
{

MeshPickObject::MeshPickObject(MeshComponent& meshComponent)
{
    owner = meshComponent.GetOwner();
    mesh = meshComponent.GetMesh();
}

MeshPickObject::~MeshPickObject() = default;

bool MeshPickObject::Intersect(
    const Ray& worldRay,
    float& nearestDistance) const
{
    if (!owner || !mesh)
        return false;

    TransformComponent* transform = owner->GetTransform();
    if (!transform)
        return false;

    const Vector3 localOrigin = transform->InvTransformPoint(worldRay.origin);
    const Vector3 localDirection = transform->InvTransformVector(worldRay.direction);

    const Ray localRay{
        .origin = localOrigin,
        .direction = localDirection
    };

    float aabbDistance;
    if (!Math::IntersectRayAABB(localRay, mesh->GetLocalBounds(), aabbDistance))
        return false;

    bool hitResult = false;

    const std::vector<Render::VertexPNT>& vertices = mesh->GetVertices();
    const std::vector<uint32>& indices = mesh->GetIndices();

    for (size_t i = 0; i + 2 < indices.size(); i += 3)
    {
        const Vector3 p0 = vertices[indices[i]].position;
        const Vector3 p1 = vertices[indices[i + 1]].position;
        const Vector3 p2 = vertices[indices[i + 2]].position;

        float distance;
        bool isHit =
            Math::IntersectLineTriangle(localRay, p0, p1, p2, distance);

        if (!isHit)
            continue;

        if (distance < nearestDistance)
        {
            nearestDistance = distance;
            hitResult = true;
        }
    }

    return hitResult;
}

void MeshPickObject::OnComponentPropertyChanged(Component* component, const Property& property)
{
    if (property.name != "Mesh")
        return;

    MeshComponent* meshComponent = static_cast<MeshComponent*>(component);
    mesh = meshComponent->GetMesh();
}

} // namespace URay
