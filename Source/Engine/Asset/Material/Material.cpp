#include "Material.h"

#include <Core/Log/LogSystem.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/EngineAsset.h>
#include <Engine/Asset/Shader/Shader.h>
#include <Engine/Asset/Texture/Texture.h>
#include <Engine/Engine.h>
#include <Render/RHI/Buffer/Buffer.h>
#include <Render/RHI/Descriptor/DescriptorSet.h>
#include <Render/RHI/Descriptor/DescriptorSetLayoutDesc.h>
#include <Render/RHI/Device.h>
#include <Render/RHI/Texture/Sampler.h>
#include <Render/RHI/Texture/Texture.h>
#include <Render/RHI/Texture/TextureView.h>
#include <Render/RenderSystem.h>
#include <Render/Rendering/RenderInfo.h>
#include <Render/ResourceManager.h>
#include <Render/Shader/Shader.h>

#include <utility>

namespace URay
{

namespace
{

bool IsCompatibleParameterType(
    Render::ShaderParameterType shaderType,
    MaterialParameterType materialType)
{
    switch (shaderType)
    {
    case Render::ShaderParameterType::Float:
        return materialType == MaterialParameterType::Float;

    case Render::ShaderParameterType::Float2:
        return materialType == MaterialParameterType::Float2;

    case Render::ShaderParameterType::Float3:
        return materialType == MaterialParameterType::Float3;

    case Render::ShaderParameterType::Float4:
        return materialType == MaterialParameterType::Float4;

    default:
        return false;
    }
}

bool IsValidParameterValue(const MaterialParameter& parameter)
{
    switch (parameter.type)
    {
    case MaterialParameterType::Float:
        return std::holds_alternative<float>(parameter.value);

    case MaterialParameterType::Float2:
        return std::holds_alternative<Vector2>(parameter.value);

    case MaterialParameterType::Float3:
        return std::holds_alternative<Vector3>(parameter.value);

    case MaterialParameterType::Float4:
        return std::holds_alternative<Color>(parameter.value);

    case MaterialParameterType::Texture2D:
        return std::holds_alternative<AssetHandle>(parameter.value);

    default:
        return false;
    }
}

bool CreateDefaultParameter(
    Render::ShaderParameterType shaderType,
    MaterialParameter& outParameter)
{
    switch (shaderType)
    {
    case Render::ShaderParameterType::Float:
        outParameter.type = MaterialParameterType::Float;
        outParameter.value = 0.0f;
        return true;

    case Render::ShaderParameterType::Float2:
        outParameter.type = MaterialParameterType::Float2;
        outParameter.value = Vector2::Zero;
        return true;

    case Render::ShaderParameterType::Float3:
        outParameter.type = MaterialParameterType::Float3;
        outParameter.value = Vector3::Zero;
        return true;

    case Render::ShaderParameterType::Float4:
        outParameter.type = MaterialParameterType::Float4;
        outParameter.value = Color::White;
        return true;

    default:
        return false;
    }
}

} // namespace

void Material::RegisterClass()
{
}

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

    descriptorSetLayout = resourceManager.GetOrCreateDescriptorSetLayout(*descriptorSetLayoutDescription);
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
        {
            Render::Sampler* sampler = resourceManager.GetOrCreateSampler({});
            if (!sampler)
                return false;

            for (uint32 i = 0; i < Render::MAX_FRAMES_IN_FLIGHT; ++i)
            {
                descriptorSets[i]->WriteSampler(binding.binding, *sampler);
            }
            break;
        }
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
            auto it = parameters.find(shaderParameter.name);

            if (it == parameters.end())
            {
                MaterialParameter defaultParameter = {};

                if (!CreateDefaultParameter(shaderParameter.type, defaultParameter))
                {
                    URAY_LOG(
                        "[Material] Unsupported shader parameter: material=%s, parameter=%s",
                        GetName().c_str(),
                        shaderParameter.name.c_str());
                    return false;
                }

                auto [insertedIt, inserted] = parameters.emplace(shaderParameter.name, std::move(defaultParameter));

                it = insertedIt;
            }

            MaterialParameter& materialParameter = it->second;

            if (!IsCompatibleParameterType(shaderParameter.type, materialParameter.type))
            {
                URAY_LOG(
                    "[Material] Parameter type mismatch: material=%s, parameter=%s",
                    GetName().c_str(),
                    shaderParameter.name.c_str());
                return false;
            }

            if (!IsValidParameterValue(materialParameter))
            {
                URAY_LOG(
                    "[Material] Invalid parameter value: material=%s, parameter=%s",
                    GetName().c_str(),
                    shaderParameter.name.c_str());
                return false;
            }

            materialParameter.binding = uniformBuffer.binding;
            materialParameter.offset = shaderParameter.offset;
            materialParameter.size = shaderParameter.size;
        }
    }

    isInitialized = true;

    return true;
}

void Material::AddParameter(const String& name, MaterialParameterType type, MaterialParameterValue value)
{
    parameters.insert({ name, MaterialParameter{
                                  .type = type,
                                  .value = value } });
}

bool Material::TryGetParameter(const String& name, MaterialParameter& outParameter) const
{
    const auto it = parameters.find(name);
    if (it == parameters.end())
        return false;

    outParameter = it->second;
    return true;
}

void Material::SetFloat(const String& name, float value)
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

void Material::SetTexture(const String& name, const AssetHandle& textureHandle)
{
    auto it = parameters.find(name);
    if (it == parameters.end())
        return;

    it->second.value = textureHandle;
    ++descriptorRevision;
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

Render::DescriptorSet* Material::GetDescriptorSet(uint32 frameIndex) const
{
    if (descriptorSets.size() <= frameIndex)
        return nullptr;

    return descriptorSets[frameIndex];
}

} // namespace URay
