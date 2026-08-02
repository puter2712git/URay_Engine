#include "DescriptorSet.h"

namespace URay
{

DescriptorSet::DescriptorSet(VkDevice device, VkDescriptorSet handle)
    : device(device), handle(handle)
{
}

DescriptorSet::~DescriptorSet()
{
}

} // namespace URay
