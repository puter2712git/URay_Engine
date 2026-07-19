#include "Mesh.h"

namespace URay
{

Mesh::~Mesh()
{
    delete indexBuffer;
    delete vertexBuffer;
}

} // namespace URay
