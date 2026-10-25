#include "Asset.h"
#include <Core/Type/Types.h>

namespace URay
{

URAY_REGISTER_CLASS(Asset)

void Asset::RegisterClass()
{
    StaticClass()->AddProperty(
        { .type = PropertyType::String,
          .name = "Name",
          .offset = offsetof(Asset, name),
          .size = sizeof(String) });
}

} // namespace URay
