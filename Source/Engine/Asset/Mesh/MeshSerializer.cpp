#include "MeshSerializer.h"

#include <Core/Binary/BinaryReader.h>
#include <Core/Binary/BinaryWriter.h>
#include <Core/File/VirtualFileSystem.h>
#include <Core/File/VirtualPath.h>
#include <Core/Type/TypeAlias.h>
#include <Engine/Asset/Mesh/MeshData.h>

namespace URay
{

constexpr uint32 MeshAssetMagic = 0x55524D53; // "URMS"
constexpr uint32 MeshAssetVersion = 1;

MeshSerializer::MeshSerializer(VirtualFileSystem& fileSystem)
    : fileSystem(fileSystem) {}

bool MeshSerializer::Save(const MeshData& mesh, const VirtualPath& assetPath) const
{
    if (!mesh.Validate())
        return false;

    BinaryWriter writer;

    writer << MeshAssetMagic;
    writer << MeshAssetVersion;

    if (!mesh.Serialize(writer))
        return false;

    const auto& buffer = writer.GetBuffer();

    return fileSystem.WriteBinary(assetPath, buffer);
}

bool MeshSerializer::Load(MeshData& outMesh, const VirtualPath& assetPath) const
{
    auto buffer = fileSystem.ReadBinary(assetPath);
    if (buffer.empty())
        return false;

    BinaryReader reader(buffer);

    uint32 magic = 0;
    reader >> magic;

    if (magic != MeshAssetMagic)
        return false;

    uint32 version = 0;
    reader >> version;

    if (version != MeshAssetVersion)
        return false;

    if (!outMesh.Deserialize(reader))
        return false;

    return outMesh.Validate();
}

} // namespace URay
