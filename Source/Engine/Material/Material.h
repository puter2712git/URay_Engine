#pragma once

#include <cstdint>
#include <vector>

namespace URay
{

class Texture;

namespace RHI
{
class Shader;
class DescriptorSetLayout;
class DescriptorSet;
class RenderDevice;
class GPUResourceManager;
} // namespace RHI

class Material
{
public:
    Material(RHI::Shader* shader);
    ~Material();

public:
    bool Initialize(RHI::RenderDevice* renderDevice, RHI::GPUResourceManager* resourceManager, Texture* defaultWhite);

    RHI::Shader* GetShader() const { return shader; }
    void SetShader(RHI::Shader* inShader) { shader = inShader; }

    Texture* GetTexture() const { return texture; }
    void SetTexture(Texture* texture);

    RHI::DescriptorSet* GetDescriptorSet(uint32_t frameIndex) const
    {
        if (descriptorSets.size() <= frameIndex)
            return nullptr;

        return descriptorSets[frameIndex];
    }

protected:
    RHI::Shader* shader = nullptr;
    Texture* texture = nullptr;

    RHI::DescriptorSetLayout* descriptorSetLayout = nullptr;
    std::vector<RHI::DescriptorSet*> descriptorSets;
};

} // namespace URay
