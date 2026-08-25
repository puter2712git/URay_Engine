#include "MaterialManager.h"

#include "Material.h"

#include "Render/GPUResourceManager.h"
#include "Render/RHI/RenderDevice.h"

namespace URay
{

MaterialManager::MaterialManager(Render::RenderDevice* renderDevice, Render::GPUResourceManager* resourceManager, Texture* defaultWhite)
    : renderDevice(renderDevice), resourceManager(resourceManager), defaultWhite(defaultWhite)
{
}

MaterialManager::~MaterialManager()
{
    for (auto& [key, material] : materials)
    {
        if (material)
        {
            delete material;
            material = nullptr;
        }
    }

    materials.clear();
}

Material* MaterialManager::GetOrCreate(const std::string& key, Render::Shader* shader)
{
    auto it = materials.find(key);
    if (it != materials.end())
        return it->second;

    Material* material = new Material(shader);
    if (!material->Initialize(renderDevice, resourceManager, defaultWhite))
    {
        delete material;
        return nullptr;
    }

    materials.insert({ key, material });

    return material;
}

} // namespace URay
