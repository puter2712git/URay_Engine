#pragma once

#include <string>
#include <unordered_map>

namespace URay
{

class Material;
class Texture;

namespace Render
{
class Shader;
class RenderDevice;
class GPUResourceManager;
} // namespace Render

class VirtualFilesystem;

class MaterialManager
{
public:
    MaterialManager(Render::RenderDevice* renderDevice, Render::GPUResourceManager* resourceManager, Texture* defaultWhite);
    ~MaterialManager();

public:
    Material* GetOrCreate(const std::string& key, Render::Shader* shader = nullptr);

    const std::unordered_map<std::string, Material*>& GetMaterials() const { return materials; }

private:
    Render::RenderDevice* renderDevice = nullptr;
    Render::GPUResourceManager* resourceManager = nullptr;

    std::unordered_map<std::string, Material*> materials;

    Texture* defaultWhite = nullptr;
};

} // namespace URay
