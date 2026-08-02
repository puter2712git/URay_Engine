#pragma once

namespace URay
{

class Shader;
class TextureAsset;

class DescriptorSetLayout;
class DescriptorSet;

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

protected:
    Shader* shader = nullptr;
    TextureAsset* texture = nullptr;

    DescriptorSet* descriptorSet = nullptr;
};

} // namespace URay
