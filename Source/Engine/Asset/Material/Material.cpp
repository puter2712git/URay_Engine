#include "Material.h"

#include "Engine/Engine.h"
#include "Engine/Asset/Texture/TextureAsset.h"

#include "Render/Descriptor/DescriptorSet.h"
#include "Render/GPUResourceManager.h"
#include "Render/RenderDevice.h"
#include "Render/RenderInfo.h"
#include "Render/Shader/Shader.h"
#include "Render/Texture/Texture.h"
#include "Render/Texture/TextureView.h"

namespace URay
{

Material::Material(Shader* shader)
    : shader(shader)
{
}

Material::~Material()
{
    for (DescriptorSet* set : descriptorSets)
    {
        if (set)
        {
            delete set;
            set = nullptr;
        }
    }

    descriptorSets.clear();
}

bool Material::Initialize(RenderDevice* renderDevice, GPUResourceManager* resourceManager)
{
    if (!renderDevice || !resourceManager || !shader)
        return false;

    const DescriptorSetLayoutDesc* setLayoutDesc = shader->GetDescriptorSetLayoutDesc(1);
    if (!setLayoutDesc)
    {
        // No needing material descriptor. This case, just return true.
        return true;
    }

    descriptorSetLayout = resourceManager->GetOrCreateDescriptorSetLayout(*setLayoutDesc);
    if (!descriptorSetLayout)
        return false;

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        DescriptorSet* set = renderDevice->CreateDescriptorSet(descriptorSetLayout);
        if (!set)
            return false;

        descriptorSets.push_back(set);
    }

    return true;
}

void Material::SetTextureAsset(TextureAsset* textureAsset)
{
    texture = textureAsset;

    GPUResourceManager* resourceManager = gEngine->GetGPUResourceManager();
    Texture* texture = resourceManager->GetOrCreateTexture(textureAsset->GetFilePath());
    TextureView* textureView = resourceManager->GetOrCreateTextureView(texture);

    for (DescriptorSet* descriptorSet : descriptorSets)
    {
        descriptorSet->WriteSampledImage(0, textureView);
        descriptorSet->WriteSampler(1, resourceManager->GetOrCreateTextureSampler({}));
    }
}

} // namespace URay
