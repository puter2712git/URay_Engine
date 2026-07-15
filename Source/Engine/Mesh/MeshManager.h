#pragma once

#include "Render/Vertex.h"

#include <unordered_map>
#include <vector>

namespace URay
{

class Renderer;
class Mesh;

class MeshManager
{
public:
    MeshManager(Renderer& renderer);
    ~MeshManager();

public:
    Mesh* CreateMesh(const std::string& key,
                     const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices);
    void RemoveMesh(const std::string& key);

    Mesh* GetMesh(const std::string& key) const;

private:
    Renderer& renderer;

    std::unordered_map<std::string, Mesh*> meshes;
};

} // namespace URay
