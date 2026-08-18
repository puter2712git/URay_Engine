#pragma once

#include <string>
#include <unordered_map>

namespace URay
{

class Material;
class Texture;

namespace RHI
{
class Shader;
class RenderDevice;
class GPUResourceManager;
} // namespace RHI

class VirtualFilesystem;

class MaterialManager
{
public:
    MaterialManager(RHI::RenderDevice* renderDevice, RHI::GPUResourceManager* resourceManager, Texture* defaultWhite);
    ~MaterialManager();

public:
    Material* GetOrCreate(const std::string& key, RHI::Shader* shader = nullptr);

    const std::unordered_map<std::string, Material*>& GetMaterials() const { return materials; }

private:
    RHI::RenderDevice* renderDevice = nullptr;
    RHI::GPUResourceManager* resourceManager = nullptr;

    std::unordered_map<std::string, Material*> materials;

    Texture* defaultWhite = nullptr;
};

} // namespace URay
