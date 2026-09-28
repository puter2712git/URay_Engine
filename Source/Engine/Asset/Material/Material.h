#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Asset/Material/MaterialParameter.h"

#include "Core/Math/Color.h"
#include "Core/Type/Types.h"

#include "Render/Rendering/RenderInfo.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace URay
{

class Shader;
class Texture;

namespace Render
{
class DescriptorSetLayoutDesc;
class DescriptorSetLayout;
class DescriptorSet;
class Buffer;
class ResourceManager;
} // namespace Render

class Material : public Asset
{
    URAY_CLASS(Material, Asset)

public:
    Material(const UUID& shaderUUID);
    ~Material();

public:
    bool Initialize();

    Shader* GetShader() const;

    void PrepareDescriptorSet(uint32 frameIndex);

    void AddParameter(const std::string& name, MaterialParameterType type, MaterialParameterValue value);

    void SetFloat(const std::string& name, float value);
    void SetTexture(const std::string& name, const UUID& textureUUID);

    Render::DescriptorSet* GetDescriptorSet(uint32 frameIndex) const
    {
        if (descriptorSets.size() <= frameIndex)
            return nullptr;

        return descriptorSets[frameIndex];
    }

protected:
    bool isInitialized = false;

    UUID shaderUUID = {};

    Render::DescriptorSetLayoutDesc* descriptorSetLayoutDescription = nullptr;
    Render::DescriptorSetLayout* descriptorSetLayout = nullptr;
    std::vector<Render::DescriptorSet*> descriptorSets;

    std::map<uint32, std::array<std::unique_ptr<Render::Buffer>, Render::MAX_FRAMES_IN_FLIGHT>> uniformBuffers;

    std::unordered_map<std::string, MaterialParameter> parameters;

    uint64 descriptorRevision = 1;
    std::array<uint64, Render::MAX_FRAMES_IN_FLIGHT> appliedDescriptorRevisions = {};
};

} // namespace URay
