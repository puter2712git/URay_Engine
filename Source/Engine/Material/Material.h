#pragma once

#include <vector>

namespace URay
{

class Shader;
class TextureAsset;

class DescriptorSetLayout;
class DescriptorSet;

class RenderDevice;
class GPUResourceManager;

class Material
{
public:
    Material(Shader* shader);
    ~Material();

public:
    bool Initialize(RenderDevice* renderDevice, GPUResourceManager* resourceManager);

    Shader* GetShader() const { return shader; }
    void SetShader(Shader* inShader) { shader = inShader; }

    DescriptorSet* GetDescriptorSet(uint32_t frameIndex) const
    {
        if (descriptorSets.size() <= frameIndex)
            return nullptr;

        return descriptorSets[frameIndex];
    }

protected:
    Shader* shader = nullptr;
    TextureAsset* texture = nullptr;

    DescriptorSetLayout* descriptorSetLayout = nullptr;
    std::vector<DescriptorSet*> descriptorSets;
};

} // namespace URay
