#pragma once

#include "Render/Descriptor/DescriptorSetLayoutDesc.h"

namespace URay
{

class Shader;
class TextureAsset;

class DescriptorSetLayout;

class Material
{
public:
    Shader* GetShader() const
    {
        return shader;
    }
    void SetShader(Shader* inShader)
    {
        shader = inShader;
    }

private:
    Shader* shader = nullptr;

    DescriptorSetLayoutDesc layoutDesc = {};
    DescriptorSetLayout* layout = nullptr;

    TextureAsset* texture = nullptr;
};

} // namespace URay
