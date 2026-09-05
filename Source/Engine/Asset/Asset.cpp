#include "Asset.h"

namespace URay
{

URAY_REGISTER_CLASS(Asset)

void Asset::RegisterClass()
{
    Super::RegisterClass();

    StaticClass()->AddProperty(
        { .type = PropertyType::String,
          .name = "Name",
          .offset = offsetof(Asset, name),
          .size = sizeof(std::string) });
}

} // namespace URay
