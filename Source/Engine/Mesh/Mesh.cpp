#include "Mesh.h"

namespace URay
{

URAY_REGISTER_CLASS(Mesh)

void Mesh::RegisterClass()
{
    Super::RegisterClass();
}

Mesh::Mesh(const std::string& name) : name(name)
{
}

} // namespace URay
