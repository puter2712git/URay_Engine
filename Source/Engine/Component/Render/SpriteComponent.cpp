#include "SpriteComponent.h"

#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Material/Material.h"
#include "Engine/Material/MaterialManager.h"
#include "Engine/Mesh/Mesh.h"
#include "Engine/Mesh/MeshManager.h"
#include "Engine/Texture/TextureManager.h"
#include "Engine/Unit.h"

#include "Core/Math/Math.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"
#include "Render/DrawCommand/DrawCommandContext.h"

namespace URay
{

URAY_REGISTER_CLASS(SpriteComponent)
URAY_REGISTER_COMPONENT(SpriteComponent)

SpriteComponent::SpriteComponent()
{
    if (!IsEnabled())
        return;

    TextureManager* textureManager = gEngine->GetTextureManager();
    texture = textureManager->GetTexture("test");

    MeshManager* meshManager = gEngine->GetMeshManager();
    quadMesh = meshManager->GetMesh("quad");

    material = gEngine->GetMaterialManager()->GetOrCreate("sprite");
    material->SetTextureAsset(texture);
}

void SpriteComponent::RegisterClass()
{
    Super::RegisterClass();

    StaticClass()->AddProperty({ PropertyType::Texture, "Texture",
                                 offsetof(SpriteComponent, texture), sizeof(TextureAsset*),
                                 [](Object* owner, const Property& property)
                                 {
                                     SpriteComponent* spriteComp = static_cast<SpriteComponent*>(owner);
                                     spriteComp->material->SetTextureAsset(spriteComp->texture);
                                 } });
}

void SpriteComponent::SubmitCommand(DrawCommandBuilder& builder)
{
    TransformComponent* transform = GetOwner()->GetTransform();

    MeshCommandContext context = {};
    context.worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity;
    context.vertexBuffer = quadMesh->GetVertexBuffer();
    context.indexBuffer = quadMesh->GetIndexBuffer();
    context.indexCount = static_cast<uint32_t>(quadMesh->GetIndices().size());
    context.material = material;

    builder.BuildFromMesh(context);
}

bool SpriteComponent::Pick(const Vector3& origin, const Vector3& direction, float& outDist) const
{
    Unit* owner = GetOwner();
    if (!owner)
        return false;

    TransformComponent* transform = owner->GetTransform();
    if (!transform)
        return false;

    const Mesh* mesh = quadMesh;
    if (!mesh)
        return false;

    const std::vector<Vertex> vertices = mesh->GetVertices();
    const std::vector<uint16_t> indices = mesh->GetIndices();

    bool isHit = false;
    float minDist = std::numeric_limits<float>::max();

    const Vector3 localOrigin = transform->InvTransformPoint(origin);
    const Vector3 localDir = transform->InvTransformVector(direction);

    for (size_t i = 0; i + 2 < indices.size(); i += 3)
    {
        const Vector3 p0 = vertices[indices[i]].pos;
        const Vector3 p1 = vertices[indices[i + 1]].pos;
        const Vector3 p2 = vertices[indices[i + 2]].pos;

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
