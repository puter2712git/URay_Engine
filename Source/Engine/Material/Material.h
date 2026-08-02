#pragma once

#include "Render/RenderInfo.h"

#include <vector>

namespace URay
{

class Shader;
class TextureAsset;

class DescriptorSetLayout;
class DescriptorSet;

class Material
{
public:
    Material(Shader* shader);

public:
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

    std::vector<DescriptorSet*> descriptorSets;
};

} // namespace URay
