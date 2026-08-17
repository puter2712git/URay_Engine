#include "Mesh.h"

namespace URay
{

URAY_REGISTER_CLASS(Mesh)

void Mesh::RegisterClass()
{
    Super::RegisterClass();
}

Mesh::Mesh(const std::string& name, const std::vector<RHI::VertexPNT>& vertices, const std::vector<uint32_t>& indices)
    : name(name), vertices(vertices), indices(indices)
{
}

} // namespace URay
