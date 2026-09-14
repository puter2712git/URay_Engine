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
class DescriptorSetLayout;
class DescriptorSet;
class Buffer;
class ResourceManager;
} // namespace Render

class Material : public Asset
{
    URAY_CLASS(Material, Asset)

public:
    Material(Shader* shader);
    ~Material();

public:
    bool Initialize();

    void SetFloat(const std::string& name, float value);
    void SetTexture(const std::string& name, Texture* texture);

    Shader* GetShader() const { return shader; }
    void SetShader(Shader* inShader) { shader = inShader; }

    Render::DescriptorSet* GetDescriptorSet(uint32 frameIndex) const
    {
        if (descriptorSets.size() <= frameIndex)
            return nullptr;

        return descriptorSets[frameIndex];
    }

protected:
    Shader* shader = nullptr;

    Render::DescriptorSetLayout* descriptorSetLayout = nullptr;
    std::vector<Render::DescriptorSet*> descriptorSets;

    std::map<uint32, std::array<std::unique_ptr<Render::Buffer>, Render::MAX_FRAMES_IN_FLIGHT>> uniformBuffers;

    std::unordered_map<std::string, MaterialParameter> parameters;
};

} // namespace URay
