#include "Material.h"

#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/Shader/Shader.h"
#include "Engine/Asset/Texture/Texture.h"
#include "Engine/Engine.h"

#include "Core/Type/Types.h"

#include "Render/RHI/Buffer/Buffer.h"
#include "Render/RHI/Descriptor/DescriptorSet.h"
#include "Render/RHI/RenderDevice.h"
#include "Render/RHI/Texture/Texture.h"
#include "Render/RHI/Texture/TextureSampler.h"
#include "Render/RHI/Texture/TextureView.h"
#include "Render/Rendering/RenderInfo.h"
#include "Render/RenderSystem.h"
#include "Render/ResourceManager.h"
#include "Render/Shader/Shader.h"

#include <utility>

namespace URay
{

void Material::RegisterClass() {}

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

    if (descriptorSetLayout)
    {
        delete descriptorSetLayout;
        descriptorSetLayout = nullptr;
    }
}

bool Material::Initialize()
{
    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Render::ResourceManager& resourceManager = renderSystem.GetResourceManager();
    Render::RenderDevice& device = renderSystem.GetDevice();
    AssetSystem& assetSystem = gEngine->GetAssetSystem();

    Render::Shader* renderShader = resourceManager.GetOrCreateShader(shader, {});
    if (!renderShader)
        return false;

    const Render::DescriptorSetLayoutDesc* layoutDescription = renderShader->GetLayoutDescription(1);
    if (!layoutDescription)
        return true;

    descriptorSetLayout = device.CreateDescriptorSetLayout(*layoutDescription);
    if (!descriptorSetLayout)
        return false;

    descriptorSets.resize(Render::MAX_FRAMES_IN_FLIGHT);
    for (uint32 i = 0; i < Render::MAX_FRAMES_IN_FLIGHT; ++i)
    {
        descriptorSets[i] = device.CreateDescriptorSet(descriptorSetLayout);
        if (!descriptorSets[i])
            return false;
    }

    for (const Render::ResourceBinding& binding : layoutDescription->bindings)
    {
        switch (binding.resourceType)
        {
        case Render::ResourceType::Sampler:
            for (uint32 i = 0; i < Render::MAX_FRAMES_IN_FLIGHT; ++i)
            {
                descriptorSets[i]->WriteSampler(binding.binding, resourceManager.GetOrCreateTextureSampler({}));
            }
            break;
        case Render::ResourceType::SampledImage:
        {
            Texture* whiteTexture = assetSystem.GetDefaultAssets().whiteTexture;
            Render::Texture* renderTexture =
                resourceManager.GetOrCreateTexture(whiteTexture);
            if (!renderTexture)
                return false;

            Render::TextureView* textureView =
                resourceManager.GetOrCreateTextureView(renderTexture);
            if (!textureView)
                return false;

            for (uint32 i = 0; i < Render::MAX_FRAMES_IN_FLIGHT; ++i)
                descriptorSets[i]->WriteSampledImage(binding.binding, textureView);

            parameters.insert({ binding.name,
                                MaterialParameter{
                                    .binding = binding.binding,
                                    .type = MaterialParameterType::Texture2D,
                                    .value = whiteTexture,
                                } });
            break;
        }
        default:
            break;
        }
    }

    for (const Render::ShaderUniformBuffer& uniformBuffer : renderShader->GetMergedReflection().uniformBuffers)
    {
        if (uniformBuffer.set != 1)
            continue;

        for (const Render::ShaderParameter& shaderParameter : uniformBuffer.parameters)
        {
            MaterialParameter materialParameter = {};
            materialParameter.binding = uniformBuffer.binding;

            materialParameter.offset = shaderParameter.offset;
            materialParameter.size = shaderParameter.size;

            parameters.insert({ shaderParameter.name, materialParameter });
        }
    }

    return true;
}

void Material::SetFloat(const std::string& name, float value)
{
    auto it = parameters.find(name);
    if (it == parameters.end())
        return;

    auto uniformBufferIt = uniformBuffers.find(it->second.binding);
    if (uniformBufferIt == uniformBuffers.end())
        return;

    if (it->second.type != MaterialParameterType::Float)
        return;

    for (uint32 i = 0; i < Render::MAX_FRAMES_IN_FLIGHT; ++i)
    {
        uniformBufferIt->second[i]->Update(&value, sizeof(float), it->second.offset);
    }
}

void Material::SetTexture(const std::string& name, Texture* texture)
{
    auto it = parameters.find(name);
    if (it == parameters.end())
        return;

    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Render::ResourceManager& resourceManager = renderSystem.GetResourceManager();

    Render::Texture* renderTexture = resourceManager.GetOrCreateTexture(texture);
    Render::TextureView* textureView = resourceManager.GetOrCreateTextureView(renderTexture);

    for (uint32 i = 0; i < Render::MAX_FRAMES_IN_FLIGHT; ++i)
    {
        descriptorSets[i]->WriteSampledImage(it->second.binding, textureView);
    }
}

} // namespace URay
