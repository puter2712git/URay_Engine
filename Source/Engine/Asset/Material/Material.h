#pragma once

#include <Core/Math/Color.h>
#include <Core/Type/TypeAlias.h>
#include <Engine/Asset/Material/MaterialInterface.h>
#include <Render/Rendering/RenderInfo.h>

#include <cstdint>

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

class Material : public MaterialInterface
{
    URAY_OBJECT(Material, MaterialInterface)

public:
    Material(const AssetHandle& shaderHandle);
    ~Material();

public:
    bool Initialize();

    Shader* GetShader() const override;

    bool TryGetParameter(const String& name, MaterialParameter& outParameter) const override;
    bool AddParameter(const String& name, MaterialParameterType type, MaterialParameterValue value);
    const HashMap<String, MaterialParameter>& GetParameters() const;

    bool SetFloat(const String& name, float value);
    bool SetFloat2(const String& name, const Vector2& value);
    bool SetFloat3(const String& name, const Vector3& value);
    bool SetFloat4(const String& name, const Color& value);
    bool SetTexture(const String& name, const AssetHandle& textureHandle);

    bool PrepareRenderMaterial(uint32 frameIndex);

    void PrepareDescriptorSet(uint32 frameIndex);
    Render::DescriptorSet* GetDescriptorSet(uint32 frameIndex) const;

protected:
    bool SetParameterValue(const String& name, MaterialParameterType expectedType, MaterialParameterValue value);

protected:
    bool isInitialized = false;

    HashMap<String, MaterialParameter> parameters;

    AssetHandle shaderHandle = {};

    Map<uint32, Array<UniquePtr<Render::Buffer>, Render::MAX_FRAMES_IN_FLIGHT>> uniformBuffers;

    uint64 descriptorRevision = 1;
    Array<uint64, Render::MAX_FRAMES_IN_FLIGHT> appliedDescriptorRevisions = {};
};

} // namespace URay
