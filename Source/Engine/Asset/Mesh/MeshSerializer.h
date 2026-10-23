#pragma once

namespace URay
{

class VirtualFileSystem;
class VirtualPath;
struct MeshData;

class MeshSerializer
{
public:
    MeshSerializer(VirtualFileSystem& fileSystem);

public:
    bool Save(const MeshData& mesh, const VirtualPath& assetPath) const;
    bool Load(MeshData& outMesh, const VirtualPath& assetPath) const;

private:
    VirtualFileSystem& fileSystem;
};

} // namespace URay
