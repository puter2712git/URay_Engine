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

URAY_REGISTER_CLASS(Material)

void Material::RegisterClass()
{
}

Material::Material(const AssetHandle& shaderHandle, MaterialRenderMode renderMode)
    : shaderHandle(shaderHandle), renderMode(renderMode)
{
}

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
                    .type = MaterialParameterType::Texture2D,
                    .value = EngineAsset::WhiteTexture });

            MaterialParameter& parameter = it->second;

            if (parameter.type != MaterialParameterType::Texture2D ||
                !std::holds_alternative<AssetHandle>(parameter.value))
            {
                return false;
            }

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

bool Material::IsInitialized() const
{
    return isInitialized;
}

Shader* Material::GetShader() const
{
    Shader* ret = nullptr;

    AssetSystem& assetSystem = gEngine->GetAssetSystem();
    AssetDatabase& assetDatabase = assetSystem.GetDatabase();

    ret = assetDatabase.Find<Shader>(shaderHandle);

    return ret;
}

MaterialRenderMode Material::GetRenderMode() const
{
    return renderMode;
}

bool Material::TryGetParameter(const String& name, MaterialParameter& outParameter) const
{
    const auto it = parameters.find(name);
    if (it == parameters.end())
        return false;

    outParameter = it->second;
    return true;
}

bool Material::AddParameter(const String& name, MaterialParameterType type, MaterialParameterValue value)
{
    MaterialParameter parameter = {};
    parameter.type = type;
    parameter.value = std::move(value);

    if (!IsValidParameterValue(parameter))
        return false;

    const auto [it, inserted] = parameters.emplace(name, std::move(parameter));

    return inserted;
}

const HashMap<String, MaterialParameter>& Material::GetParameters() const
{
    return parameters;
}

bool Material::SetFloat(const String& name, float value)
{
    return SetParameterValue(name, MaterialParameterType::Float, value);
}

bool Material::SetFloat2(const String& name, const Vector2& value)
{
    return SetParameterValue(name, MaterialParameterType::Float2, value);
}

bool Material::SetFloat3(const String& name, const Vector3& value)
{
    return SetParameterValue(name, MaterialParameterType::Float3, value);
}

bool Material::SetFloat4(const String& name, const Color& value)
{
    return SetParameterValue(name, MaterialParameterType::Float4, value);
}

bool Material::SetTexture(const String& name, const AssetHandle& textureHandle)
{
    return SetParameterValue(name, MaterialParameterType::Texture2D, textureHandle);
}

bool Material::SetParameterValue(const String& name, MaterialParameterType expectedType, MaterialParameterValue value)
{
    auto it = parameters.find(name);
    if (it == parameters.end())
        return false;

    MaterialParameter& parameter = it->second;

    if (parameter.type != expectedType)
        return false;

    MaterialParameter newParameter = {};
    newParameter.type = expectedType;
    newParameter.value = std::move(value);

    if (!IsValidParameterValue(newParameter))
        return false;

    parameter.value = std::move(newParameter.value);

    return true;
}

} // namespace URay
