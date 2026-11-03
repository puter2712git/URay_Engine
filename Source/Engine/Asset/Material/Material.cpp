#include "Material.h"

#include <Core/Log/LogSystem.h>
#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/AssetSystem.h>
#include <Engine/Asset/EngineAsset.h>
#include <Engine/Asset/Material/MaterialStateBuilder.h>
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
#include <Render/Rendering/Material/Material.h>
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

bool ValidateMaterialReflection(const Render::ShaderReflection& reflection)
{
    constexpr uint32 materialSet = 1;

    HashSet<String> parameterNames;

    for (const Render::ShaderUniformBuffer& buffer : reflection.uniformBuffers)
    {
        if (buffer.set != materialSet)
            continue;

        for (const Render::ShaderParameter& parameter : buffer.parameters)
        {
            if (parameter.name.empty() ||
                !parameterNames.insert(parameter.name).second)
            {
                URAY_LOG(
                    "[Material] Empty or duplicate shader parameter name: %s",
                    parameter.name.c_str());
                return false;
            }

            uint32 expectedSize = 0;

            switch (parameter.type)
            {
            case Render::ShaderParameterType::Float:
                expectedSize = sizeof(float);
                break;

            case Render::ShaderParameterType::Float2:
                expectedSize = sizeof(float) * 2;
                break;

            case Render::ShaderParameterType::Float3:
                expectedSize = sizeof(float) * 3;
                break;

            case Render::ShaderParameterType::Float4:
                expectedSize = sizeof(float) * 4;
                break;

            default:
                URAY_LOG(
                    "[Material] Unsupported shader parameter type: %s",
                    parameter.name.c_str());
                return false;
            }

            if (parameter.offset > buffer.size ||
                parameter.size > buffer.size - parameter.offset ||
                expectedSize > parameter.size)
            {
                URAY_LOG(
                    "[Material] Invalid uniform buffer range: %s",
                    parameter.name.c_str());
                return false;
            }
        }
    }

    for (const Render::ShaderDescriptorBinding& binding : reflection.descriptorBindings)
    {
        if (binding.set != materialSet ||
            binding.type != Render::ResourceType::SampledImage)
        {
            continue;
        }

        if (binding.name.empty() ||
            !parameterNames.insert(binding.name).second)
        {
            URAY_LOG(
                "[Material] Empty or duplicate shader parameter name: %s",
                binding.name.c_str());
            return false;
        }

        if (binding.count != 1)
        {
            URAY_LOG(
                "[Material] Texture arrays are not supported: %s",
                binding.name.c_str());
            return false;
        }
    }

    return true;
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

Material::~Material() = default;

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

    const Render::DescriptorSetLayoutDesc* layoutDesc = renderShader->GetLayoutDescription(1);
    if (!layoutDesc)
    {
        isInitialized = true;
        return true;
    }

    if (!ValidateMaterialReflection(renderShader->GetMergedReflection()))
    {
        URAY_LOG(
            "[Material] Invalid shader reflection: material=%s",
            GetName().c_str());
        return false;
    }

    for (const Render::ResourceBinding& binding : layoutDesc->bindings)
    {
        switch (binding.resourceType)
        {
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

    Render::Material* renderMaterial = resourceManager.GetOrCreateMaterial(this);

    if (!renderMaterial ||
        !renderMaterial->Initialize(
            device,
            resourceManager,
            *layoutDesc,
            renderShader->GetMergedReflection()))
    {
        URAY_LOG(
            "[Material] Failed to initialize render material: %s",
            GetName().c_str());
        return false;
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

const HashMap<String, MaterialParameter>& Material::GetParameters() const
{
    return parameters;
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

bool Material::PrepareRenderMaterial(uint32 frameIndex)
{
    if (!isInitialized)
        return false;

    if (frameIndex >= Render::MAX_FRAMES_IN_FLIGHT)
        return false;

    Shader* shader = GetShader();
    if (!shader)
        return false;

    Render::RenderSystem& renderSystem = gEngine->GetRenderSystem();
    Render::ResourceManager& resourceManager = renderSystem.GetResourceManager();

    Render::Shader* renderShader = resourceManager.GetOrCreateShader(shader, {});
    if (!renderShader)
        return false;

    if (!renderShader->GetLayoutDescription(1))
        return true;

    Render::Material* renderMaterial = resourceManager.GetOrCreateMaterial(this);
    if (!renderMaterial)
        return false;

    AssetDatabase& assetDB = gEngine->GetAssetSystem().GetDatabase();

    Render::MaterialState materialState = {};

    if (!BuildMaterialState(
            *this,
            renderShader->GetMergedReflection(),
            assetDB,
            resourceManager,
            materialState))
    {
        return false;
    }

    renderMaterial->Update(materialState);

    return renderMaterial->Prepare(frameIndex);
}

void Material::PrepareDescriptorSet(uint32 frameIndex)
{
    if (appliedDescriptorRevisions[frameIndex] == descriptorRevision)
        return;

    Render::DescriptorSet* descriptorSet = GetDescriptorSet(frameIndex);
    if (!descriptorSet)
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

        descriptorSet->WriteSampledImage(parameter.binding, view);
    }

    appliedDescriptorRevisions[frameIndex] = descriptorRevision;
}

Render::DescriptorSet* Material::GetDescriptorSet(uint32 frameIndex) const
{
    Render::ResourceManager& resourceManager = gEngine->GetRenderSystem().GetResourceManager();

    Render::Material* renderMaterial = resourceManager.GetOrCreateMaterial(const_cast<Material*>(this));
    return renderMaterial ? renderMaterial->GetDescriptorSet(frameIndex) : nullptr;
}

} // namespace URay
