#include "TextureView.h"

namespace URay::Render
{

TextureView::TextureView(VkDevice device, VkImageView handle, Texture* ref)
    : device(device), handle(handle), refTexture(ref)
{
}

TextureView::~TextureView()
{
    if (handle)
    {
        vkDestroyImageView(device, handle, nullptr);
    }
}

} // namespace URay::Render
