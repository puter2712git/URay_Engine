#include "MaterialManager.h"

#include "Material.h"

namespace URay
{

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

Material* MaterialManager::GetOrCreate(const std::string& key, Shader* shader)
{
    auto it = materials.find(key);
    if (it != materials.end())
        return it->second;

    Material* material = new Material();
    material->SetShader(shader);

    materials.insert({ key, material });

    return material;
}

} // namespace URay
