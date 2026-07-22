#pragma once

#include "Render/Vertex.h"

#include <string>
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
    bool CreateDefaultMeshes();

    Mesh* CreateMesh(const std::string& key,
                     const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices);

    Mesh* GetMesh(const std::string& key) const;

private:
    void CreateBox();
    void CreateQuad();
    void CreateCylinder();
    void CreateCone();
    void CreateArrow();

private:
    Renderer& renderer;

    std::unordered_map<std::string, Mesh*> meshes;
};

} // namespace URay
