#pragma once

#include "Render/Vertex.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace URay
{

class RenderDevice;
class Mesh;

class MeshManager
{
public:
    MeshManager(RenderDevice* device);
    ~MeshManager();

public:
    bool CreateDefaultMeshes();

    Mesh* CreateMesh(const std::string& key,
                     const std::vector<Vertex>& vertices, const std::vector<uint16_t>& indices);

    Mesh* GetMesh(const std::string& key) const;
    const std::unordered_map<std::string, Mesh*>& GetMeshes() const { return meshes; }

private:
    void CreateBox();
    void CreateQuad();
    void CreateCylinder();
    void CreateCone();
    void CreateArrow();
    void CreateRotationGizmo();
    void CreateScaleGizmo();

private:
    RenderDevice* device = nullptr;

    std::unordered_map<std::string, Mesh*> meshes;
};

} // namespace URay
