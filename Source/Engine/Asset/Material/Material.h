#pragma once

#include <Core/Math/Color.h>
#include <Core/Type/TypeAlias.h>
#include <Engine/Asset/Asset.h>
#include <Engine/Asset/Material/MaterialParameter.h>
#include <Render/Rendering/RenderInfo.h>

#include <cstdint>
#include <unordered_map>

namespace URay
{

class Shader;
class Texture;

namespace Render
{
struct DescriptorSetLayoutDesc;
class DescriptorSetLayout;
class DescriptorSet;
class Buffer;
class ResourceManager;
} // namespace Render

class Material : public Asset
{
    URAY_OBJECT(Material, Asset)

public:
    Material(const AssetHandle& shaderHandle);
    ~Material();

public:
    bool Initialize();

    void AddParameter(const String& name, MaterialParameterType type, MaterialParameterValue value);
    bool TryGetParameter(const String& name, MaterialParameter& outParameter) const;

    void SetFloat(const String& name, float value);
    void SetTexture(const String& name, const AssetHandle& textureHandle);

    Shader* GetShader() const;

    void PrepareDescriptorSet(uint32 frameIndex);
    Render::DescriptorSet* GetDescriptorSet(uint32 frameIndex) const;

protected:
    bool isInitialized = false;

    HashMap<String, MaterialParameter> parameters;

    AssetHandle shaderHandle = {};

    std::map<uint32, Array<UniquePtr<Render::Buffer>, Render::MAX_FRAMES_IN_FLIGHT>> uniformBuffers;

    Render::DescriptorSetLayoutDesc* descriptorSetLayoutDescription = nullptr;
    Render::DescriptorSetLayout* descriptorSetLayout = nullptr;
    Vector<Render::DescriptorSet*> descriptorSets;

    uint64 descriptorRevision = 1;
    Array<uint64, Render::MAX_FRAMES_IN_FLIGHT> appliedDescriptorRevisions = {};
};

} // namespace URay
