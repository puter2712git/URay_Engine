#include "TextureView.h"

namespace URay::Render
{

TextureView::TextureView(VkDevice device, VkImageView handle, Texture* ref, const TextureViewDesc& viewDesc)
    : device(device), handle(handle), refTexture(ref), desc(viewDesc)
{
}

TextureView::~TextureView()
{
    vkDestroyImageView(device, handle, nullptr);
}

} // namespace URay::Render
