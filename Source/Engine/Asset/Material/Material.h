#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Asset/Material/MaterialParameter.h"

#include "Core/Math/Color.h"
#include "Core/Type/Types.h"

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
class RenderDevice;
class ResourceManager;
} // namespace Render

class Material : public Asset
{
    URAY_CLASS(Material, Asset)

public:
    Material(Shader* shader);
    ~Material();

public:
    bool Initialize(Render::RenderDevice* renderDevice, Render::ResourceManager* resourceManager, Texture* defaultWhite);

    void SetParameter(const std::string& name, MaterialParameterValue value);
    const MaterialParameterValue* GetParameter(const std::string& name) const;
    const MaterialParameterDesc* GetParameterDesc(const std::string& name) const;

    Shader* GetShader() const { return shader; }
    void SetShader(Shader* inShader) { shader = inShader; }

    Render::DescriptorSet* GetDescriptorSet(uint32 frameIndex) const
    {
        if (descriptorSets.size() <= frameIndex)
            return nullptr;

        return descriptorSets[frameIndex];
    }

protected:
    bool ApplyParameter(
        const MaterialParameterDesc& desc,
        const MaterialParameterValue& value);

protected:
    Shader* shader = nullptr;

    std::unordered_map<std::string, MaterialParameterValue> parameters;
    std::unordered_map<std::string, MaterialParameterDesc> parameterDescs;

    Render::ResourceManager* resourceManager = nullptr;
    Render::DescriptorSetLayout* descriptorSetLayout = nullptr;
    std::vector<Render::DescriptorSet*> descriptorSets;
};

} // namespace URay
