#pragma once

#include "Render/Vertex.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace URay
{

class RenderDevice;
class MeshAsset;

class MeshManager
{
public:
    MeshManager();
    ~MeshManager();

public:
    bool CreateDefaultMeshes();

    MeshAsset* CreateMesh(const std::string& key,
                          const std::vector<VertexPNT>& vertices, const std::vector<uint32_t>& indices);

    MeshAsset* GetMesh(const std::string& key) const;
    const std::unordered_map<std::string, MeshAsset*>& GetMeshes() const { return meshes; }

private:
    void CreateBox();
    void CreateQuad();
    void CreateCylinder();
    void CreateCone();
    void CreateArrow();
    void CreateRotationGizmo();
    void CreateScaleGizmo();

private:
    std::unordered_map<std::string, MeshAsset*> meshes;
};

} // namespace URay
