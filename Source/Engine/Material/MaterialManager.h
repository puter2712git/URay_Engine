#pragma once

#include <string>
#include <unordered_map>

namespace URay
{

class Material;
class Shader;

class MaterialManager
{
public:
    ~MaterialManager();

public:
    Material* GetOrCreate(const std::string& key, Shader* shader = nullptr);

private:
    std::unordered_map<std::string, Material*> materials;
};

} // namespace URay
