#include "Material.h"

#include "Engine/Asset/Texture/Texture.h"
#include "Engine/Engine.h"

#include "Core/Type/Types.h"

#include "Render/GPUResourceManager.h"
#include "Render/RHI/Descriptor/DescriptorSet.h"
#include "Render/RHI/RenderDevice.h"
#include "Render/RHI/Texture/Texture.h"
#include "Render/RHI/Texture/TextureView.h"
#include "Render/RenderInfo.h"
#include "Render/RenderSystem.h"
#include "Render/Shader/Shader.h"

namespace URay
{

Material::Material(Render::Shader* shader)
    : shader(shader)
{
}

Material::~Material()
{
    for (Render::DescriptorSet* set : descriptorSets)
    {
        if (set)
        {
            delete set;
            set = nullptr;
        }
    }

    descriptorSets.clear();
}

void Material::RegisterClass()
{
    Super::RegisterClass();
}

bool Material::Initialize(Render::RenderDevice* renderDevice, Render::GPUResourceManager* resourceManager, Texture* defaultWhite)
{
    if (!renderDevice || !resourceManager || !shader)
        return false;

    const Render::DescriptorSetLayoutDesc* setLayoutDesc = shader->GetDescriptorSetLayoutDesc(1);
    if (!setLayoutDesc)
    {
        // No needing material descriptor. This case, just return true.
        return true;
    }

    descriptorSetLayout = resourceManager->GetOrCreateDescriptorSetLayout(*setLayoutDesc);
    if (!descriptorSetLayout)
        return false;

    for (uint32 i = 0; i < Render::MAX_FRAMES_IN_FLIGHT; ++i)
    {
        Render::DescriptorSet* set = renderDevice->CreateDescriptorSet(descriptorSetLayout);
        if (!set)
            return false;

        descriptorSets.push_back(set);
    }

    SetTexture(texture ? texture : defaultWhite);

    return true;
}

void Material::SetTexture(Texture* textureAsset)
{
    texture = textureAsset;

    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Render::GPUResourceManager& resourceManager = renderSystem.GetResourceManager();
    Render::Texture* texture = resourceManager.GetOrCreateTexture(textureAsset);
    Render::TextureView* textureView = resourceManager.GetOrCreateTextureView(texture);

    if (!textureView)
        return;

    for (Render::DescriptorSet* descriptorSet : descriptorSets)
    {
        descriptorSet->WriteSampledImage(0, textureView);
        descriptorSet->WriteSampler(1, resourceManager.GetOrCreateTextureSampler({}));
    }
}

} // namespace URay
