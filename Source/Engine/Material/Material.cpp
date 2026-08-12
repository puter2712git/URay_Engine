#include "Material.h"

#include "Engine/Engine.h"
#include "Engine/Texture/Texture.h"

#include "Render/Descriptor/DescriptorSet.h"
#include "Render/GPUResourceManager.h"
#include "Render/RenderDevice.h"
#include "Render/RenderInfo.h"
#include "Render/Shader/Shader.h"
#include "Render/Texture/Texture.h"
#include "Render/Texture/TextureView.h"

namespace URay
{

Material::Material(RHI::Shader* shader)
    : shader(shader)
{
}

Material::~Material()
{
    for (RHI::DescriptorSet* set : descriptorSets)
    {
        if (set)
        {
            delete set;
            set = nullptr;
        }
    }

    descriptorSets.clear();
}

bool Material::Initialize(RHI::RenderDevice* renderDevice, RHI::GPUResourceManager* resourceManager)
{
    if (!renderDevice || !resourceManager || !shader)
        return false;

    const RHI::DescriptorSetLayoutDesc* setLayoutDesc = shader->GetDescriptorSetLayoutDesc(1);
    if (!setLayoutDesc)
    {
        // No needing material descriptor. This case, just return true.
        return true;
    }

    descriptorSetLayout = resourceManager->GetOrCreateDescriptorSetLayout(*setLayoutDesc);
    if (!descriptorSetLayout)
        return false;

    for (uint32_t i = 0; i < RHI::MAX_FRAMES_IN_FLIGHT; ++i)
    {
        RHI::DescriptorSet* set = renderDevice->CreateDescriptorSet(descriptorSetLayout);
        if (!set)
            return false;

        descriptorSets.push_back(set);
    }

    return true;
}

void Material::SetTexture(Texture* textureAsset)
{
    texture = textureAsset;

    RHI::GPUResourceManager* resourceManager = gEngine->GetGPUResourceManager();
    RHI::Texture* texture = resourceManager->GetOrCreateTexture(textureAsset);
    RHI::TextureView* textureView = resourceManager->GetOrCreateTextureView(texture);

    for (RHI::DescriptorSet* descriptorSet : descriptorSets)
    {
        descriptorSet->WriteSampledImage(0, textureView);
        descriptorSet->WriteSampler(1, resourceManager->GetOrCreateTextureSampler({}));
    }
}

} // namespace URay
