#include "MaterialStateBuilder.h"

#include <Engine/Asset/AssetDatabase.h>
#include <Engine/Asset/EngineAsset.h>
#include <Engine/Asset/Material/Material.h>
#include <Engine/Asset/Texture/Texture.h>
#include <Render/Rendering/Material/Material.h>
#include <Render/ResourceManager.h>
#include <Render/Shader/Reflection/ShaderReflection.h>

#include <cstring>
#include <utility>

namespace URay
{

namespace
{

bool WriteUniformParameter(
    const Render::ShaderParameter& shaderParameter,
    const MaterialParameter& materialParameter,
    Vector<uint8>& buffer)
{
    float values[4] = {};
    uint32 valueSize = 0;

    switch (shaderParameter.type)
    {
    case Render::ShaderParameterType::Float:
    {
        const float* value = std::get_if<float>(&materialParameter.value);

        if (materialParameter.type != MaterialParameterType::Float || !value)
            return false;

        values[0] = *value;
        valueSize = sizeof(float);
        break;
    }

    case Render::ShaderParameterType::Float2:
    {
        const Vector2* value = std::get_if<Vector2>(&materialParameter.value);

        if (materialParameter.type != MaterialParameterType::Float2 || !value)
            return false;

        values[0] = value->x;
        values[1] = value->y;
        valueSize = sizeof(float) * 2;
        break;
    }

    case Render::ShaderParameterType::Float3:
    {
        const Vector3* value = std::get_if<Vector3>(&materialParameter.value);

        if (materialParameter.type != MaterialParameterType::Float3 || !value)
            return false;

        values[0] = value->x;
        values[1] = value->y;
        values[2] = value->z;
        valueSize = sizeof(float) * 3;
        break;
    }

    case Render::ShaderParameterType::Float4:
    {
        const Color* value = std::get_if<Color>(&materialParameter.value);

        if (materialParameter.type != MaterialParameterType::Float4 || !value)
            return false;

        values[0] = value->r;
        values[1] = value->g;
        values[2] = value->b;
        values[3] = value->a;
        valueSize = sizeof(float) * 4;
        break;
    }

    default:
        return false;
    }

    const size_t offset = shaderParameter.offset;

    if (valueSize > shaderParameter.size ||
        offset > buffer.size() ||
        valueSize > buffer.size() - offset)
    {
        return false;
    }

    std::memcpy(buffer.data() + offset, values, valueSize);
    return true;
}

bool BuildUniformBufferStates(
    const Material& material,
    const Render::ShaderReflection& reflection,
    Vector<Render::MaterialUniformBufferState>& outBuffers)
{
    for (const Render::ShaderUniformBuffer& uniformBuffer : reflection.uniformBuffers)
    {
        if (uniformBuffer.set != 1)
            continue;

        Render::MaterialUniformBufferState bufferState = {};
        bufferState.binding = uniformBuffer.binding;
        bufferState.data.resize(uniformBuffer.size, 0);

        for (const Render::ShaderParameter& shaderParameter : uniformBuffer.parameters)
        {
            MaterialParameter parameter = {};

            if (!material.TryGetParameter(shaderParameter.name, parameter))
                return false;

            if (!WriteUniformParameter(shaderParameter, parameter, bufferState.data))
                return false;
        }

        outBuffers.push_back(std::move(bufferState));
    }

    return true;
}

bool BuildTextureStates(
    const Material& material,
    const Render::ShaderReflection& reflection,
    AssetDatabase& assetDB,
    Render::ResourceManager& resourceManager,
    Vector<Render::MaterialTextureState>& outTextures)
{
    for (const Render::ShaderDescriptorBinding& binding : reflection.descriptorBindings)
    {
        if (binding.set != 1 ||
            binding.type != Render::ResourceType::SampledImage)
        {
            continue;
        }

        if (binding.count != 1)
            return false;

        MaterialParameter parameter = {};

        if (!material.TryGetParameter(binding.name, parameter))
            return false;

        if (parameter.type != MaterialParameterType::Texture2D)
            return false;

        const AssetHandle* textureHandle = std::get_if<AssetHandle>(&parameter.value);
        if (!textureHandle)
            return false;

        Texture* texture = assetDB.Find<Texture>(*textureHandle);

        if (!texture)
            texture = assetDB.Find<Texture>(EngineAsset::WhiteTexture);

        if (!texture)
            return false;

        Render::Texture* renderTexture = resourceManager.GetOrCreateTexture(texture);
        if (!renderTexture)
            return false;

        Render::TextureView* view = resourceManager.GetOrCreateTextureView(renderTexture, {});
        if (!view)
            return false;

        outTextures.push_back({ .binding = binding.binding,
                                .view = view });
    }

    return true;
}

} // namespace

bool BuildMaterialState(
    const Material& material,
    const Render::ShaderReflection& reflection,
    AssetDatabase& assetDB,
    Render::ResourceManager& resourceManager,
    Render::MaterialState& outState)
{
    Render::MaterialState newState = {};

    if (!BuildUniformBufferStates(
            material,
            reflection,
            newState.uniformBuffers))
    {
        return false;
    }

    if (!BuildTextureStates(
            material,
            reflection,
            assetDB,
            resourceManager,
            newState.textures))
    {
        return false;
    }

    outState = std::move(newState);
    return true;
}

} // namespace URay
