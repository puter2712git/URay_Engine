#pragma once

#include "Engine/Asset/Asset.h"

#include "Core/Math/Color.h"
#include "Core/Type/Types.h"

#include <cstdint>
#include <vector>

namespace URay
{

class Texture;

namespace Render
{
class Shader;
class DescriptorSetLayout;
class DescriptorSet;
class RenderDevice;
class GPUResourceManager;
} // namespace Render

class Material : public Asset
{
    URAY_CLASS(Material, Asset)

public:
    Material(Render::Shader* shader);
    ~Material();

public:
    bool Initialize(Render::RenderDevice* renderDevice, Render::GPUResourceManager* resourceManager, Texture* defaultWhite);

    Render::Shader* GetShader() const { return shader; }
    void SetShader(Render::Shader* inShader) { shader = inShader; }

    Texture* GetTexture() const { return texture; }
    void SetTexture(Texture* texture);

    const Color& GetBaseColor() const { return baseColor; }
    void SetBaseColor(const Color& inBaseColor) { baseColor = inBaseColor; }

    Render::DescriptorSet* GetDescriptorSet(uint32 frameIndex) const
    {
        if (descriptorSets.size() <= frameIndex)
            return nullptr;

        return descriptorSets[frameIndex];
    }

protected:
    Render::Shader* shader = nullptr;
    Texture* texture = nullptr;
    Color baseColor = Color::White;

    Render::DescriptorSetLayout* descriptorSetLayout = nullptr;
    std::vector<Render::DescriptorSet*> descriptorSets;
};

} // namespace URay
