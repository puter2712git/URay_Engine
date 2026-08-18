#include "SpriteComponent.h"

#include "Engine/Component/TransformComponent.h"
#include "Engine/Engine.h"
#include "Engine/Material/Material.h"
#include "Engine/Material/MaterialManager.h"
#include "Engine/Mesh/Mesh.h"
#include "Engine/Mesh/MeshManager.h"
#include "Engine/Texture/TextureManager.h"
#include "Engine/Unit.h"

#include "Render/DrawCommand/DrawCommandBuilder.h"
#include "Render/DrawCommand/DrawCommandContext.h"

namespace URay
{

using namespace RHI;

URAY_REGISTER_CLASS(SpriteComponent)
URAY_REGISTER_COMPONENT(SpriteComponent)

SpriteComponent::SpriteComponent()
{
    TextureManager* textureManager = gEngine->GetTextureManager();
    texture = textureManager->GetTexture("Test");

    MeshManager* meshManager = gEngine->GetMeshManager();
    quadMesh = meshManager->GetMesh("Quad");

    material = gEngine->GetMaterialManager()->GetOrCreate("Sprite");
    material->SetTexture(texture);
}

void SpriteComponent::RegisterClass()
{
    Super::RegisterClass();

    StaticClass()->AddProperty({ .type = PropertyType::Texture,
                                 .name = "Texture",
                                 .offset = offsetof(SpriteComponent, texture),
                                 .size = sizeof(Texture*),
                                 .OnChangedCallback = [](Object* owner, const Property& property)
                                 {
                                     SpriteComponent* spriteComp = static_cast<SpriteComponent*>(owner);
                                     spriteComp->material->SetTexture(spriteComp->texture);
                                 } });
}

void SpriteComponent::SubmitCommand(DrawCommandBuilder& builder)
{
    TransformComponent* transform = GetOwner()->GetTransform();

    builder.BuildFromMesh({
        .worldMatrix = transform ? transform->GetWorldMatrix() : Matrix::Identity,
        .mesh = quadMesh,
        .material = material,
        .indexOffset = 0,
        .indexCount = static_cast<uint32_t>(quadMesh->GetIndices().size()),
    });
}

} // namespace URay
