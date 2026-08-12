#include "MaterialManager.h"

#include "Material.h"

#include "Render/GPUResourceManager.h"
#include "Render/RenderDevice.h"

namespace URay
{

MaterialManager::MaterialManager(RHI::RenderDevice* renderDevice, RHI::GPUResourceManager* resourceManager)
    : renderDevice(renderDevice), resourceManager(resourceManager)
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

Material* MaterialManager::GetOrCreate(const std::string& key, RHI::Shader* shader)
{
    auto it = materials.find(key);
    if (it != materials.end())
        return it->second;

    Material* material = new Material(shader);
    if (!material->Initialize(renderDevice, resourceManager))
    {
        delete material;
        return nullptr;
    }

    materials.insert({ key, material });

    return material;
}

} // namespace URay
