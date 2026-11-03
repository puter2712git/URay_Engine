#pragma once

#include <Core/Type/TypeAlias.h>
#include <Render/Rendering/RenderInfo.h>

namespace URay::Render
{

class Buffer;
class DescriptorSet;
class Device;
class ResourceManager;
class TextureView;
struct DescriptorSetLayoutDesc;
struct ShaderReflection;

struct MaterialUniformBufferState
{
    uint32 binding = 0;
    Vector<uint8> data;
};

struct MaterialTextureState
{
    uint32 binding = 0;
    TextureView* view = nullptr;
};

struct MaterialState
{
    Vector<MaterialUniformBufferState> uniformBuffers;
    Vector<MaterialTextureState> textures;
};

class Material
{
public:
    Material();
    ~Material();

    Material(const Material&) = delete;
    Material& operator=(const Material&) = delete;

public:
    bool Initialize(
        Device& device,
        ResourceManager& resourceManager,
        const DescriptorSetLayoutDesc& layoutDesc,
        const ShaderReflection& reflection);

    void Update(const MaterialState& newState);

    bool Prepare(uint32 frameIndex);

    DescriptorSet* GetDescriptorSet(uint32 frameIndex) const;

private:
    bool initialized = false;

    MaterialState state = {};

    Map<uint32, Array<UniquePtr<Buffer>, MAX_FRAMES_IN_FLIGHT>> uniformBuffers;
    HashSet<uint32> textureBindings;

    Array<UniquePtr<DescriptorSet>, MAX_FRAMES_IN_FLIGHT> descriptorSets;
};

} // namespace URay::Render
