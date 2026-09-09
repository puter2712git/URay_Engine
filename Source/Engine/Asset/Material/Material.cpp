#include "Material.h"

#include "Engine/Asset/Shader/Shader.h"
#include "Engine/Asset/Texture/Texture.h"
#include "Engine/Engine.h"

#include "Core/Type/Types.h"

#include "Render/RHI/Descriptor/DescriptorSet.h"
#include "Render/RHI/RenderDevice.h"
#include "Render/RHI/Texture/Texture.h"
#include "Render/RHI/Texture/TextureSampler.h"
#include "Render/RHI/Texture/TextureView.h"
#include "Render/RenderInfo.h"
#include "Render/RenderSystem.h"
#include "Render/ResourceManager.h"
#include "Render/Shader/Shader.h"

#include <utility>

namespace URay
{

Material::Material(Shader* shader) : shader(shader) {}

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

bool Material::Initialize(Render::RenderDevice* renderDevice, Render::ResourceManager* resourceManager, Texture* defaultWhite)
{
    if (!renderDevice || !resourceManager || !shader)
        return false;

    Render::Shader* renderShader = resourceManager->GetOrCreateShader(shader, {});
    if (!renderShader)
        return false;

    const Render::DescriptorSetLayoutDesc* setLayoutDesc = renderShader->GetDescriptorSetLayoutDesc(1);
    if (!setLayoutDesc)
    {
        // No needing material descriptor. This case, just return true.
        return true;
    }

    std::vector<uint32> samplerBindings;
    parameterDescs.clear();

    for (const Render::ReflectedDescriptorBinding& binding
         : renderShader->GetPipelineReflection().descriptorBindings)
    {
        if (binding.set != 1)
            continue;

        if (binding.isRuntimeArray || binding.arrayCount != 1)
            return false;

        switch (binding.resourceType)
        {
        case Render::ShaderResourceType::SampledImage:
        {
            if (binding.name.empty() || parameterDescs.contains(binding.name))
                return false;

            parameterDescs.insert({
                binding.name,
                MaterialParameterDesc {
                    .name = binding.name,
                    .type = MaterialParameterType::Texture2D,
                    .set = binding.set,
                    .binding = binding.binding,
                },
            });
            break;
        }
        case Render::ShaderResourceType::Sampler:
            samplerBindings.push_back(binding.binding);
            break;
        default:
            return false;
        }
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

    this->resourceManager = resourceManager;

    if (!samplerBindings.empty())
    {
        const VkSampler sampler = resourceManager->GetOrCreateTextureSampler({});
        if (sampler == VK_NULL_HANDLE)
            return false;

        for (Render::DescriptorSet* descriptorSet : descriptorSets)
        {
            for (uint32 binding : samplerBindings)
                descriptorSet->WriteSampler(binding, sampler);
        }
    }

    for (const auto& [name, desc] : parameterDescs)
    {
        parameters.try_emplace(name, MaterialParameterValue {
            .type = MaterialParameterType::Texture2D,
            .value = defaultWhite,
        });

        if (!ApplyParameter(desc, parameters.at(name)))
            return false;
    }

    return true;
}

void Material::SetParameter(const std::string& name, MaterialParameterValue value)
{
    const auto descIt = parameterDescs.find(name);
    if (resourceManager && descIt != parameterDescs.end())
    {
        if (!ApplyParameter(descIt->second, value))
            return;
    }

    parameters.insert_or_assign(name, std::move(value));
}

const MaterialParameterValue* Material::GetParameter(const std::string& name) const
{
    const auto it = parameters.find(name);
    if (it == parameters.end())
        return nullptr;

    return &it->second;
}

const MaterialParameterDesc* Material::GetParameterDesc(const std::string& name) const
{
    const auto it = parameterDescs.find(name);
    if (it == parameterDescs.end())
        return nullptr;

    return &it->second;
}

bool Material::ApplyParameter(
    const MaterialParameterDesc& desc,
    const MaterialParameterValue& value)
{
    if (!resourceManager || desc.type != value.type)
        return false;

    switch (desc.type)
    {
    case MaterialParameterType::Texture2D:
    {
        Texture* const* textureAsset = std::get_if<Texture*>(&value.value);
        if (!textureAsset || !*textureAsset)
            return false;

        Render::Texture* texture = resourceManager->GetOrCreateTexture(*textureAsset);
        if (!texture)
            return false;

        Render::TextureView* textureView = resourceManager->GetOrCreateTextureView(texture);
        if (!textureView)
            return false;

        for (Render::DescriptorSet* descriptorSet : descriptorSets)
            descriptorSet->WriteSampledImage(desc.binding, textureView);

        return true;
    }
    default:
        return false;
    }
}

// void Material::SetTexture(Texture* textureAsset)
//{
//     texture = textureAsset;
//
//     Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
//     Render::ResourceManager& resourceManager = renderSystem.GetResourceManager();
//     Render::Texture* texture = resourceManager.GetOrCreateTexture(textureAsset);
//     Render::TextureView* textureView = resourceManager.GetOrCreateTextureView(texture);
//
//     if (!textureView)
//         return;
//
//     for (Render::DescriptorSet* descriptorSet : descriptorSets)
//     {
//         descriptorSet->WriteSampledImage(0, textureView);
//         descriptorSet->WriteSampler(1, resourceManager.GetOrCreateTextureSampler({}));
//     }
// }

} // namespace URay
