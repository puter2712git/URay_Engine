#include "Material.h"

#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Asset/AssetSystem.h"
#include "Engine/Asset/EngineAsset.h"
#include "Engine/Asset/Shader/Shader.h"
#include "Engine/Asset/Texture/Texture.h"
#include "Engine/Engine.h"

#include "Core/Type/Types.h"

#include "Render/RHI/Buffer/Buffer.h"
#include "Render/RHI/Descriptor/DescriptorSet.h"
#include "Render/RHI/Descriptor/DescriptorSetLayoutDesc.h"
#include "Render/RHI/Device.h"
#include "Render/RHI/Texture/Sampler.h"
#include "Render/RHI/Texture/Texture.h"
#include "Render/RHI/Texture/TextureView.h"
#include "Render/RenderSystem.h"
#include "Render/Rendering/RenderInfo.h"
#include "Render/ResourceManager.h"
#include "Render/Shader/Shader.h"

#include <utility>

namespace URay
{

void Material::RegisterClass() {}

Material::Material(const AssetHandle& shaderHandle) : shaderHandle(shaderHandle) {}

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
    if (isInitialized)
        return true;

    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Render::ResourceManager& resourceManager = renderSystem.GetResourceManager();
    Render::Device& device = renderSystem.GetDevice();
    AssetSystem& assetSystem = gEngine->GetAssetSystem();

    Shader* shader = GetShader();

    Render::Shader* renderShader = resourceManager.GetOrCreateShader(shader, {});
    if (!renderShader)
        return false;

    descriptorSetLayoutDescription = const_cast<Render::DescriptorSetLayoutDesc*>(renderShader->GetLayoutDescription(1));
    if (!descriptorSetLayoutDescription)
        return true;

    descriptorSetLayout = device.CreateDescriptorSetLayout(*descriptorSetLayoutDescription);
    if (!descriptorSetLayout)
        return false;

    descriptorSets.resize(Render::MAX_FRAMES_IN_FLIGHT);
    for (uint32 i = 0; i < Render::MAX_FRAMES_IN_FLIGHT; ++i)
    {
        descriptorSets[i] = device.CreateDescriptorSet(descriptorSetLayout);
        if (!descriptorSets[i])
            return false;
    }

    for (const Render::ResourceBinding& binding : descriptorSetLayoutDescription->bindings)
    {
        switch (binding.resourceType)
        {
        case Render::ResourceType::Sampler:
            for (uint32 i = 0; i < Render::MAX_FRAMES_IN_FLIGHT; ++i)
            {
                descriptorSets[i]->WriteSampler(binding.binding, resourceManager.GetOrCreateSampler({}));
            }
            break;
        case Render::ResourceType::SampledImage:
        {
            auto [it, inserted] = parameters.try_emplace(
                binding.name,
                MaterialParameter{
                    .binding = binding.binding,
                    .type = MaterialParameterType::Texture2D,
                    .value = EngineAsset::WhiteTexture });

            MaterialParameter& parameter = it->second;

            if (parameter.type != MaterialParameterType::Texture2D ||
                !std::holds_alternative<AssetHandle>(parameter.value))
            {
                return false;
            }

            parameter.binding = binding.binding;
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

    isInitialized = true;

    return true;
}

Shader* Material::GetShader() const
{
    Shader* ret = nullptr;

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();

    ret = assetDatabase.Find<Shader>(shaderHandle);

    return ret;
}

void Material::PrepareDescriptorSet(uint32 frameIndex)
{
    if (appliedDescriptorRevisions[frameIndex] == descriptorRevision)
        return;

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();
    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Render::ResourceManager& resourceManager = renderSystem.GetResourceManager();

    for (const auto& [name, parameter] : parameters)
    {
        if (parameter.type != MaterialParameterType::Texture2D)
            continue;

        const AssetHandle textureHandle = std::get<AssetHandle>(parameter.value);

        Texture* texture = assetDatabase.Find<Texture>(textureHandle);
        if (!texture)
            texture = assetDatabase.Find<Texture>(EngineAsset::WhiteTexture);

        Render::Texture* renderTexture = resourceManager.GetOrCreateTexture(texture);
        Render::TextureView* view = resourceManager.GetOrCreateTextureView(renderTexture, {});

        descriptorSets[frameIndex]->WriteSampledImage(parameter.binding, view);
    }

    appliedDescriptorRevisions[frameIndex] = descriptorRevision;
}

void Material::AddParameter(const std::string& name, MaterialParameterType type, MaterialParameterValue value)
{
    parameters.insert({ name, MaterialParameter{
                                  .type = type,
                                  .value = value } });
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

void Material::SetTexture(const std::string& name, const AssetHandle& textureHandle)
{
    auto it = parameters.find(name);
    if (it == parameters.end())
        return;

    it->second.value = textureHandle;
    ++descriptorRevision;
}

} // namespace URay
