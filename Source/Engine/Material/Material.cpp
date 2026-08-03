#include "Material.h"

#include "Render/Descriptor/DescriptorSet.h"
#include "Render/GPUResourceManager.h"
#include "Render/RenderDevice.h"
#include "Render/RenderInfo.h"
#include "Render/Shader/Shader.h"

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

} // namespace URay
