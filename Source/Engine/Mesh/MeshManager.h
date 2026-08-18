#pragma once

#include "Render/Vertex.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace URay
{

class RenderDevice;
class Mesh;
class Material;

class MeshManager
{
public:
    MeshManager();
    ~MeshManager();

public:
    bool CreateDefaultMeshes(Material* defaultMaterial);

    Mesh* CreateMesh(const std::string& key,
                     const std::vector<RHI::VertexPNT>& vertices, const std::vector<uint32_t>& indices);

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
    std::unordered_map<std::string, Mesh*> meshes;
};

} // namespace URay
