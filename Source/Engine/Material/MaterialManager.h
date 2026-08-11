#pragma once

#include <string>
#include <unordered_map>

namespace URay
{

class Material;
class Shader;

class RenderDevice;
class GPUResourceManager;

class VirtualFilesystem;

class MaterialManager
{
public:
    MaterialManager(RenderDevice* renderDevice, GPUResourceManager* resourceManager);
    ~MaterialManager();

public:
    Material* GetOrCreate(const std::string& key, Shader* shader = nullptr);

    const std::unordered_map<std::string, Material*>& GetMaterials() const { return materials; }

private:
    RenderDevice* renderDevice = nullptr;
    GPUResourceManager* resourceManager = nullptr;

    std::unordered_map<std::string, Material*> materials;
};

} // namespace URay
