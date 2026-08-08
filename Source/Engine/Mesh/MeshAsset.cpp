#include "MeshAsset.h"

namespace URay
{

MeshAsset::~MeshAsset()
{
    delete indexBuffer;
    delete vertexBuffer;
}

} // namespace URay
